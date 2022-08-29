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

#include <inspector/trace_event.hpp>

#include <inspector/details/config.hpp>
#include <inspector/details/event_queue.hpp>
#include <inspector/details/logging.hpp>
#include <inspector/details/shared_object.hpp>

namespace inspector {
namespace details {

/**
 * @brief The class `TraceWriter` is used to publish trace events to the shared
 * event queue.
 *
 */
class TraceWriter {
 public:
  /**
   * @brief Get instance of writer.
   *
   */
  static TraceWriter& Get();

  /**
   * @brief Destroy the TraceWriter object.
   *
   * The DTOR marks the event queue for removal if the remove flag was set in
   * the configuration settings.
   *
   */
  ~TraceWriter();

  /**
   * @brief Write the given event to the shared event queue.
   *
   * @param event Constant reference to the event.
   */
  void Write(const std::string& event);

 private:
  /**
   * @brief Construct a new TraceWriter object.
   *
   */
  TraceWriter();

  const bool remove_;
  const std::size_t max_attempt_;
  const std::string queue_name_;
  details::EventQueue* queue_;
};

// -----------------------------------
// Writer Implementation
// -----------------------------------

inline TraceWriter::TraceWriter()
    : remove_(details::Config::Get().queue_remove_on_exit),
      max_attempt_(details::Config::Get().read_max_attempt),
      queue_name_(details::Config::Get().queue_system_unique_name),
      queue_(details::shared_object::GetOrCreate<details::EventQueue>(
          queue_name_)) {}

// ----------- public -----------------

// static
inline TraceWriter& TraceWriter::Get() {
  static TraceWriter writer;
  return writer;
}

inline TraceWriter::~TraceWriter() {
  if (remove_) {
    LOG_INFO << "Marking the shared event queue for removal.";
    details::shared_object::Remove(queue_name_);
  }
}

inline void TraceWriter::Write(const std::string& event) {
  auto status = queue_->Publish(event, max_attempt_);
  if (status == details::EventQueue::Status::FULL) {
    LOG_ERROR << "Event Queue Full: Unable to write trace event.";
  }
}

// -----------------------------------

/**
 * @brief Write the given trace event onto the event queue.
 *
 * @param event Constant reference to the trace event.
 */
inline void WriteTraceEvent(const TraceEvent& event) {
  TraceWriter::Get().Write(event.String());
}

/**
 * @brief Construct and write a trace event with the given arguments.
 *
 */
template <class... Args>
inline void WriteTraceEvent(const char type, const std::string& name,
                            const Args&... args) {
  if (inspector::details::Config::Get().disable_tracing) {
    return;
  }
  TraceEvent event(type, name);
  event.SetArgs(args...);
  WriteTraceEvent(event);
}

}  // namespace details
}  // namespace inspector