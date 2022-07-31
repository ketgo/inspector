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

#include <inspector/details/config.hpp>
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
   * @brief Construct a new Writer object.
   *
   * @param remove Flag for removal of the queue on DTOR.
   * @param max_attempt Maximum number of attempts to make when consuming event.
   * @param queue_name System unique name of the shared event queue.
   */
  Writer(
      const bool remove = false,
      const std::size_t max_attempt = details::EventQueue::defaultMaxAttempt(),
      const std::string& queue_name = details::kEventQueueSystemUniqueName);

  /**
   * @brief Destroy the Writer object.
   *
   * The DTOR marks the event queue for removal if the remove flag was set in
   * the CTOR.
   *
   */
  ~Writer();

  /**
   * @brief Write the given event to the shared event queue.
   *
   * @param event Constant reference to the event.
   */
  void Write(const std::string& event);

 private:
  const bool remove_;
  const std::size_t max_attempt_;
  const std::string queue_name_;
  details::EventQueue* queue_;
};

// -----------------------------------
// Writer Implementation
// -----------------------------------

Writer::Writer(const bool remove, const std::size_t max_attempt,
               const std::string& queue_name)
    : remove_(remove),
      max_attempt_(max_attempt),
      queue_name_(queue_name),
      queue_(details::shared_object::GetOrCreate<details::EventQueue>(
          queue_name_)) {}

Writer::~Writer() {
  if (remove_) {
    LOG_INFO << "Marking the shared event queue for removal.";
    details::shared_object::Remove(queue_name_);
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