/**
 * Copyright 2023 Ketan Goyal
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

#include <inspector/debug_args.hpp>
#include <inspector/types.hpp>
#include <string>
#include <vector>

namespace inspector {

/**
 * @brief Non-mutable view into a recorded trace event.
 *
 */
class TraceEvent {
 public:
  /**
   * @brief Construct a new TraceEvent object.
   *
   * Default CTOR creating an empty trace event.
   *
   */
  TraceEvent() = default;

  /**
   * @brief Construct a new TraceEvent object.
   *
   * @param buffer Rvalue reference to the buffer containing the trace event.
   */
  explicit TraceEvent(std::vector<uint8_t>&& buffer);

  /**
   * @brief Check if the trace event is empty.
   *
   * @returns `true` if empty else `false`.
   */
  bool isEmpty() const;

  /**
   * @brief Get the type of trace event.
   *
   * @returns Trace event type.
   * @throws `std::runtime_error` if the event is empty.
   */
  event_type_t type() const;

  /**
   * @brief Get the trace event counter.
   *
   * @returns Counter value of trace event.
   * @throws `std::runtime_error` if the event is empty.
   */
  uint64_t counter() const;

  /**
   * @brief Get the timestamp in nanoseconds of the trace event.
   *
   * @returns Timestamp in nanoseconds.
   * @throws `std::runtime_error` if the event is empty.
   */
  timestamp_t timestampNs() const;

  /**
   * @brief Get the process identifier.
   *
   * @returns Process identifier.
   * @throws `std::runtime_error` if the event is empty.
   */
  int32_t pid() const;

  /**
   * @brief Get the thread identifier.
   *
   * @returns Thread identifier.
   * @throws `std::runtime_error` if the event is empty.
   */
  int32_t tid() const;

  /**
   * @brief Get the name of trace event.
   *
   * @returns Name of trace event as a c-string.
   * @throws `std::runtime_error` if the event is empty.
   */
  const char* name() const;

  /**
   * @brief Get the debug arguments which are part of the trace event.
   *
   * @returns Object of type `DebugArgs`.
   * @throws `std::runtime_error` if the event is empty.
   */
  DebugArgs debugArgs() const;

  /**
   * @brief Get JSON string representation of the trace event.
   *
   * @returns JSON string representation.
   */
  std::string toJson() const;

 private:
  std::vector<uint8_t> buffer_;
};

}  // namespace inspector