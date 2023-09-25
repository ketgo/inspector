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
#include <new>

#include <inspector/details/serializer.hpp>

#define PACKED __attribute__((packed))

namespace inspector {

/**
 * @brief Data struct containing keyword arguments and values to be stored as
 * part of an event.
 *
 * @tparam T Type of value.
 */
template <class T>
struct KwArgs {
  const char* name;  //<- Name of argument.
  const T& value;    //<- Value of argument.
};

/**
 * @brief The data structure `Event` contains all the information of a
 * single captured trace or metric event using the inspector library.
 *
 * TODO: Copy CTOR and copy assignment operator.
 *
 */
class PACKED Event {
 public:
  /**
   * @brief Get the memory size in bytes required to create an event with
   * given additional arguments.
   *
   * @tparam Args Type of arguments to be stored as part of the event.
   * @param args Constant reference to the arguments.
   * @returns Storage size in bytes.
   */
  template <class... Args>
  static size_t memorySize(const Args&... args) {
    return sizeof(Event) + details::storageSize(args...);
  }

  /**
   * @brief Construct an Event object at the given memory address.
   *
   * @tparam Args Type of additional arguments.
   * @param address Memory address at which to construct the Event object.
   * @param type Type of event.
   * @param category Category of the event.
   * @param timestamp Timestamp of the event.
   * @param pid Process identifier of the event.
   * @param tid Thread identifier of the event.
   * @param args Constant reference to additional arguments.
   * @return Pointer to the constructed Event object.
   *
   * @warning Its the users responsibility to ensure enough memory size is
   * available at the given address to construct an Event object. Th required
   * memory size can be obtained using the `Event::memorySize` method.
   */
  template <class... Args>
  static Event* constructAt(void* address, const uint8_t type,
                            const uint8_t category, const int64_t timestamp,
                            const int32_t pid, const int32_t tid,
                            const Args&... args) {
    return ::new (const_cast<void*>(static_cast<const volatile void*>(address)))
        Event(type, category, timestamp, pid, tid, args...);
  }

  /**
   * @brief Get the type of the event.
   *
   * @returns Type of event.
   */
  uint8_t type() const;

  /**
   * @brief Get the category of the event.
   *
   * @returns Category of event.
   */
  uint8_t category() const;

  /**
   * @brief Get the timestamp of the event.
   *
   * @returns Timestamp of event.
   */
  int64_t timestamp() const;

  /**
   * @brief Get the process ID.
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
   * @brief Get the number of arguments in the event.
   *
   * @returns Number of arguments.
   */
  uint32_t argsCount() const;

 private:
  /**
   * @brief Construct a new Event object
   *
   * @tparam Args Type of additional arguments.
   * @param type Type of event.
   * @param category Category of the event.
   * @param timestamp Timestamp of the event.
   * @param pid Process identifier of the event.
   * @param tid Thread identifier of the event.
   * @param args Constant reference to additional arguments.
   */
  template <class... Args>
  Event(const uint8_t type, const uint8_t category, const int64_t timestamp,
        const int32_t pid, const int32_t tid, const Args&... args)
      : type_(type),
        category_(category),
        timestamp_(timestamp),
        pid_(pid),
        tid_(tid),
        args_count_(sizeof...(Args)) {
    details::dump(args_, args...);
  }

  uint8_t type_;       //<- Type of event. This field is used to distinguish the
                       // different events.
  uint8_t category_;   //<- Category of event.
  int64_t timestamp_;  //<- Timestamp in nano seconds when the event occurred.
  int32_t pid_;  //<- Identifier of the process in which the event occurred or
                 // is observed.
  int32_t tid_;  //<- Identifier of the thread in which the event occurred or is
                 // observed.
  uint32_t args_count_;  //<- Number of arguments in the event.
  uint8_t args_[];       //<- Additional arguments stored in the event.
};

}  // namespace inspector