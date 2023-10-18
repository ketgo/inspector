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

#include <vector>

#include <inspector/debug_args.hpp>
#include <inspector/types.hpp>

namespace inspector {

class TraceEvent {
 public:
  /**
   * @brief Construct a new TraceEvent object.
   *
   * @param buffer Rvalue reference to the buffer containing the trace event.
   */
  explicit TraceEvent(std::vector<uint8_t>&& buffer);

  /**
   * @brief Get the type of trace event.
   *
   * @returns Trace event type.
   */
  event_type_t type() const;

  /**
   * @brief Get the trace event counter.
   *
   * @returns Counter value of trace event.
   */
  uint64_t counter() const;

  /**
   * @brief Get the timestamp in nanoseconds of the trace event.
   *
   * @returns Timestamp in nanoseconds.
   */
  timestamp_t timestampNs() const;

  /**
   * @brief Get the process identifier.
   *
   * @returns Process identifier.
   */
  int32_t pid() const;

  /**
   * @brief Get the thread identifier.
   *
   * @returns Thread identifier.
   */
  int32_t tid() const;

  /**
   * @brief Get the name of trace event.
   *
   * @returns Name of trace event as a c-string.
   */
  const char* name() const;

  /**
   * @brief Get the debug arguments which are part of the trace event.
   *
   * @returns Object of type `DebugArgs`.
   */
  DebugArgs debugArgs() const;

 private:
  std::vector<uint8_t> buffer_;
};

}  // namespace inspector