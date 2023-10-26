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
 * @brief The class `DebugArg` represents a debug argument.
 *
 */
class DebugArg {
  friend class DebugArgs;

 public:
  /**
   * @brief Enumerated set of different debug argument types.
   *
   */
  enum class Type {
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
   * @brief Get the type of the debug argument.
   *
   * @returns Type enum.
   */
  Type type() const;

  /**
   * @brief Get the value of the debug argument.
   *
   * @tparam T Data type of the argument.
   * @returns Value of the argument.
   */
  template <class T>
  T value() const;

  /**
   * @brief Get the storage location of the debug argument.
   *
   * @returns Pointer to the memory location.
   */
  const void* address() const;

 private:
  /**
   * @brief Construct a new DebugArg object.
   *
   * @param address Memory location where the argument is stored.
   */
  explicit DebugArg(const void* const address);

  const void* address_;
};

/**
 * @brief Collection of debug arguments present in a trace event.
 *
 */
class DebugArgs {
  friend class TraceEvent;

 public:
  /**
   * @brief Iterator for reading arguments in a trace event.
   *
   */
  class Iterator {
    friend class DebugArgs;

   public:
    Iterator();

    const DebugArg& operator*() const;
    const DebugArg* operator->() const;
    Iterator& operator++();
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;

   private:
    Iterator(const void* const address, const size_t count);

    DebugArg debug_arg_;
    size_t count_;
  };

  /**
   * @brief Construct a new DebugArgs object.
   *
   */
  DebugArgs();

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
  /**
   * @brief Construct a new DebugArgs object
   *
   * @param address Starting address of the arguments.
   * @param storage_size Number of bytes used to store the arguments.
   * @param count Number of arguments.
   */
  DebugArgs(const void* const address, const size_t storage_size,
            const size_t count);

  const void* address_;
  size_t storage_size_;
  size_t count_;
};

}  // namespace inspector