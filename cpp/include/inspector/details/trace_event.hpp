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
   * @param size Size of the memory buffer.
   */
  MutableTraceEvent(void* const address, const size_t size);

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
   * @brief Append the given debug argument to the event.
   *
   * @tparam T Type of debug argument.
   * @param arg Constant reference to the debug argument.
   */
  template <class T>
  void appendDebugArg(const T& arg);

  /**
   * @brief Append the given string literal debug argument.
   *
   * @tparam N Length of string literal.
   */
  template <std::size_t N>
  void appendDebugArg(const char (&arg)[N]) {
    appendDebugArg<const char*>(arg);
  }

  /**
   * @brief Apeend the given keyword debug argument.
   *
   * @tparam T Type of argument value.
   */
  template <class T>
  void appendDebugArg(const KeywordArg<T>& arg) {
    appendKeywordName(arg.name);
    appendDebugArg(arg.value);
  }

  /**
   * @brief Append the given multiple debug arguments to the trace event.
   *
   * @tparam T Type of first debug argument.
   * @tparam Args Type of other debug arguments.
   * @param arg Constant reference to the first debug argument.
   * @param args Constant reference to rest of the debug arguments.
   */
  template <class T, class... Args>
  void appendDebugArgs(const T& arg, const Args&... args) {
    appendDebugArg(arg);
    appendDebugArgs(args...);
  }

 private:
  // Dummy method to facilitate template parameter expansion
  void appendDebugArgs() {}
  // Append keyword argument name.
  void appendKeywordName(const char* const name);

  void* address_;
  const size_t size_;
  void* debug_args_head_;
};

}  // namespace details
}  // namespace inspector