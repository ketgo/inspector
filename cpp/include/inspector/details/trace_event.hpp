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

#include <inspector/details/debug_args.hpp>
#include <inspector/types.hpp>

namespace inspector {
namespace details {

/**
 * @brief Get the storage size in bytes required to store a trace event with no
 * debug arguments.
 *
 * @returns Size in bytes.
 */
size_t traceEventStorageSize();

/**
 * @brief Get the size in bytes required to store a trace event with multiple
 * debug arguments.
 *
 * @tparam Args Argument types to store as part of the event.
 * @param args Constant reference to the arguments.
 * @returns Size in bytes.
 */
template <class... Args>
size_t traceEventStorageSize(const Args&... args) {
  return traceEventStorageSize() + debugArgsStorageSize(args...);
}

/**
 * @brief The class `MutableTraceEvent` casts a memory buffer as a mutable
 * trace event in order to efficiently create trace events.
 *
 */
class MutableTraceEvent {
 public:
  /**
   * @brief Construct a new MutableEventView object.
   *
   * @param address Pointer to starting address of memory buffer.
   */
  explicit MutableTraceEvent(void* const address);

  /**
   * @brief Set the type of trace event.
   *
   * @param type Trace event type.
   */
  void setType(const event_type_t type);

  /**
   * @brief Set the trace event counter.
   *
   * @param counter Counter value to set.
   */
  void setCounter(const uint64_t counter);

  /**
   * @brief Set the timestamp in nanoseconds of the trace event.
   *
   * @param timestamp_ns Timestamp in nanoseconds.
   */
  void setTimestampNs(const timestamp_t timestamp_ns);

  /**
   * @brief Set the process identifier.
   *
   * @param pid Process identifier.
   */
  void setPid(const int32_t pid);

  /**
   * @brief Set the thread identifier.
   *
   * @param tid Thread identifier.
   */
  void setTid(const int32_t tid);

  /**
   * @brief Get the number of debug arguments stored in the trace event.
   *
   * @returns Number of debug arguments
   */
  uint8_t debugArgsCount() const;

  /**
   * @brief Add the given arguments to the event.
   *
   * @tparam Args Argument types to store as part of the event.
   * @param args Constant reference to the arguments.
   */
  template <class... Args>
  void addDebugArgs(const Args&... args) {
    addDebugArgsAt(0, args...);
  }

 private:
  template <class T>
  size_t addDebugArgAt(const size_t offset, const T& arg);

  // Dummy method to facilitate template parameter expansion
  void addDebugArgsAt(const size_t offset) {}

  template <class T, class... Args>
  void addDebugArgsAt(const size_t offset, const T& arg, const Args&... args) {
    addDebugArgsAt(offset + addDebugArgAt(offset, arg), args...);
  }

  void* address_;
};

}  // namespace details
}  // namespace inspector