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

#include <cstdint>
#include <cstddef>

namespace inspector {

/**
 * @brief Enumerated set of debug argument types which can be added to a trace
 * event.
 *
 */
enum class DebugArgType {
  TYPE_INT16 = 0,
  TYPE_INT32,
  TYPE_INT64,
  TYPE_UINT8,
  TYPE_UINT16,
  TYPE_UINT32,
  TYPE_UINT64,
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_CHAR,
  TYPE_STRING,
};

/**
 * @brief Type of trace event data type.
 *
 */
using event_type_t = uint8_t;

/**
 * @brief Category of trace event data type.
 *
 */
using event_category_t = uint8_t;

/**
 * @brief Timestamp data type.
 *
 */
using timestamp_t = int64_t;

namespace details {
namespace TraceEvent {

/**
 * @brief The class `DebugArgs` represents collection of debug arguments in a
 * trace event.
 *
 */
class DebugArgs {
 public:
  /**
   * @brief Get the size in bytes required to store no debug arguments. Pretty
   * lame method which always return 0. The method is needed to facilitate
   * template parameter expansion.
   *
   */
  static size_t storageSize() { return 0; }

  /**
   * @brief Get the size in bytes required to store the given debug argument in
   * a trace event.
   *
   * @tparam T Type of argument to store.
   * @param arg Constant reference to argument.
   * @returns Size in bytes.
   */
  template <class T>
  static size_t storageSize(const T& arg);

  /**
   * @brief Get the size in bytes required to store the given multiple debug
   * arguments in a trace event.
   *
   * @tparam T Type of first argument.
   * @tparam Args Rest of the argument types to store as part of the event.
   * @param arg Constant reference to the first argument.
   * @param args Constant reference to the rest of the arguments.
   * @returns Size in bytes.
   */
  template <class T, class... Args>
  static size_t storageSize(const T& arg, const Args&... args) {
    return storageSize(arg) + storageSize(args...);
  }

  /**
   * @brief Iterator for reading arguments in a trace event.
   *
   */
  class Iterator {
    friend class DebugArgs;
    friend class View;

   public:
    Iterator();

    /**
     * @brief Get the type of argument pointed to by the iterator.
     *
     * @returns DebugArgType enum.
     */
    DebugArgType type() const;

    /**
     * @brief Get the argument pointed to by the iterator.
     *
     * @tparam T Type of argument.
     * @returns Constant reference to the argument.
     */
    template <class T>
    T get() const;

    Iterator& operator++();
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;

   private:
    Iterator(const DebugArgs& debug_args, const void* const address,
             const size_t count);

    const DebugArgs* debug_args_;
    const void* current_;
    size_t counts_;
  };

  /**
   * @brief Construct a new DebugArgs object.
   *
   * @param address Memory address to the first debug argument.
   * @param counts Number of debug arguments.
   */
  DebugArgs(const void* const address, const size_t counts);

  /**
   * @brief Get the number of debug arguments.
   *
   * @returns Number of debug arguments.
   */
  size_t size() const;

  /**
   * @brief Get iterator to the starting debug argument.
   *
   * @returns Iterator object.
   */
  Iterator begin() const;

  /**
   * @brief Get iterator to the one past the last debug argument.
   *
   * @returns Iterator object.
   */
  Iterator end() const;

 private:
  const void* address_;
  size_t counts_;
};

/**
 * @brief Get the storage size in bytes required to store a trace event with no
 * debug arguments.
 *
 * @returns Size in bytes.
 */
size_t storageSize();

/**
 * @brief Get the size in bytes required to store a trace event with multiple
 * debug arguments.
 *
 * @tparam Args Argument types to store as part of the event.
 * @param args Constant reference to the arguments.
 * @returns Size in bytes.
 */
template <class... Args>
size_t storageSize(const Args&... args) {
  return storageSize() + DebugArgs::storageSize(args...);
}

/**
 * @brief The class `View` exposes a memory buffer as constant trace event. It
 * is internally used to efficiently expose a trace event.
 *
 */
class View {
 public:
  /**
   * @brief Construct a new MutableEventView object.
   *
   * @param address Pointer to starting address of memory buffer.
   */
  explicit View(const void* const address);

  /**
   * @brief Get the type of trace event.
   *
   * @returns Trace event type.
   */
  event_type_t type() const;

  /**
   * @brief Get the category of trace event.
   *
   * @returns Category of trace event.
   */
  event_category_t category() const;

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
  const void* address_;
};

/**
 * @brief The class `MutableView` exposes a memory buffer as a mutable trace
 * event. It is used internally to efficiently create trace events.
 *
 */
class MutableView {
 public:
  /**
   * @brief Construct a new MutableEventView object.
   *
   * @param address Pointer to starting address of memory buffer.
   */
  explicit MutableView(void* const address);

  /**
   * @brief Set the type of trace event.
   *
   * @param type Trace event type.
   */
  void setType(const event_type_t type);

  /**
   * @brief Get the type of trace event.
   *
   * @returns Trace event type.
   */
  event_type_t type() const;

  /**
   * @brief Set the category of trace event.
   *
   * @param category Category of trace event.
   */
  void setCategory(const event_category_t category);

  /**
   * @brief Get the category of trace event.
   *
   * @returns Category of trace event.
   */
  event_category_t category() const;

  /**
   * @brief Set the trace event counter.
   *
   * @param counter Counter value to set.
   */
  void setCounter(const uint64_t counter);

  /**
   * @brief Get the trace event counter.
   *
   * @returns Counter value of trace event.
   */
  uint64_t counter() const;

  /**
   * @brief Set the timestamp in nanoseconds of the trace event.
   *
   * @param timestamp_ns Timestamp in nanoseconds.
   */
  void setTimestampNs(const timestamp_t timestamp_ns);

  /**
   * @brief Get the timestamp in nanoseconds of the trace event.
   *
   * @returns Timestamp in nanoseconds.
   */
  timestamp_t timestampNs() const;

  /**
   * @brief Set the process identifier.
   *
   * @param pid Process identifier.
   */
  void setPid(const int32_t pid);

  /**
   * @brief Get the process identifier.
   *
   * @returns Process identifier.
   */
  int32_t pid() const;

  /**
   * @brief Set the thread identifier.
   *
   * @param tid Thread identifier.
   */
  void setTid(const int32_t tid);

  /**
   * @brief Get the thread identifier.
   *
   * @returns Thread identifier.
   */
  int32_t tid() const;

  /**
   * @brief Set the name of the trace event.
   *
   * @returns Name of the trace event as c-string.
   */
  const char* name() const;

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

}  // namespace TraceEvent
}  // namespace details
}  // namespace inspector