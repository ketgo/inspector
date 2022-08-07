/**
 * Copyright 2022 Ketan Goyal
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <inspector/config.hpp>
#include <inspector/details/event_queue.hpp>
#include <inspector/details/logging.hpp>
#include <inspector/details/shared_object.hpp>

namespace inspector {

/**
 * @brief The class `Writer` can be used to publish trace or metrics events to
 * the shared event queue.
 *
 */
class Writer {
 public:
  /**
   * @brief Span encapsulating memory block in the event queue to write an
   * event.
   *
   */
  using Span = details::EventQueue::WriteSpan;

  /**
   * @brief Set writer configuration.
   *
   * NOTE: Writer configuration must be set before any method or classes in the
   * inpector is used.
   *
   * @param config Constant reference to the new configuration.
   */
  static void SetConfig(const Config& config);

  /**
   * @brief Construct a new Writer object.
   *
   */
  Writer();

  /**
   * @brief Destroy the Writer object.
   *
   * The DTOR marks the event queue for removal if the remove flag was set in
   * the CTOR.
   *
   */
  ~Writer();

  /**
   * @brief Reserve space in the event queue to write an event of given size.
   *
   * The method sets the passed span object so that it can be used to write the
   * event onto the queue.
   *
   * @param span Reference to the span.
   * @param size Number of bytes to write.
   */
  void Reserve(Span& span, const std::size_t size);

  /**
   * @brief Write the given event to the shared event queue.
   *
   * @param event Constant reference to the event.
   */
  void Write(const std::string& event);

 private:
  /**
   * @brief Get writer configuration instance.
   *
   */
  static Config& ConfigInstance();

  const bool remove_;
  const std::size_t max_attempt_;
  const std::string queue_name_;
  details::EventQueue* queue_;
};

// -----------------------------------
// Writer Implementation
// -----------------------------------

// static
inline Config& Writer::ConfigInstance() {
  static Config config;
  return config;
}

// ------------- public --------------

// static
inline void Writer::SetConfig(const Config& config) {
  ConfigInstance() = config;
}

Writer::Writer()
    : remove_(ConfigInstance().remove),
      max_attempt_(ConfigInstance().max_attempt),
      queue_name_(ConfigInstance().queue_system_unique_name),
      queue_(details::shared_object::GetOrCreate<details::EventQueue>(
          queue_name_)) {}

Writer::~Writer() {
  if (remove_) {
    LOG_INFO << "Marking the shared event queue for removal.";
    details::shared_object::Remove(queue_name_);
  }
}

void Writer::Reserve(Span& span, const std::size_t size) {
  auto status = queue_->Reserve(span, size, max_attempt_);
  if (status == details::EventQueue::Status::FULL) {
    LOG_ERROR << "Unable to write event as the shared event queue is full.";
  }
}

void Writer::Write(const std::string& event) {
  auto status = queue_->Publish(event, max_attempt_);
  if (status == details::EventQueue::Status::FULL) {
    LOG_ERROR << "Unable to write event as the shared event queue is full.";
  }
}

// -----------------------------------

}  // namespace inspector