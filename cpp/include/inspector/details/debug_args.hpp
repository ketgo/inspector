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

#include <cstddef>
#include <cstdint>

namespace inspector {
namespace details {

/**
 * @brief Wrapper struct for appending keyword arguments.
 *
 */
template <class T>
struct KeywordArg {
  const char* name;
  T value;

  KeywordArg(const char* const _name, const T& _value)
      : name(_name), value(_value) {}
};

/**
 * @brief Utility method to create keyword argument.
 *
 */
template <class T>
KeywordArg<T> makeKeywordArg(const char* const name, const T& value) {
  return {name, value};
}

/**
 * @brief Get the storage size in bytes required to store given debug argument.
 *
 * @tparam T Type of argument to store.
 * @param arg Constant reference to argument.
 * @returns Size in bytes.
 */
template <class T>
size_t debugArgStorageSize(const T& arg);

/**
 * @brief Get the storage size in bytes required to store given string literal
 * debug argument.
 *
 * @tparam N Number of characters in the string literal.
 * @param arg Constant reference to argument.
 * @returns Size in bytes.
 */
template <std::size_t N>
size_t debugArgStorageSize(const char (&arg)[N]) {
  return sizeof(uint8_t) + sizeof(arg);
}

/**
 * @brief Get the storage size in bytes required to store given keyword
 * debug argument.
 *
 * @tparam T Type of value.
 * @param arg Constant reference to argument.
 * @returns Size in bytes.
 */
template <class T>
size_t debugArgStorageSize(const KeywordArg<T>& arg) {
  // NOTE: The type marker for name is used for keyword argument type instead of
  // using another byte. This saves space.
  return debugArgStorageSize(arg.name) + debugArgStorageSize(arg.value);
}

/**
 * @brief Get the size in bytes required to store no debug arguments. Pretty
 * lame method which always return 0. The method is needed to facilitate
 * template parameter expansion.
 *
 */
inline size_t debugArgsStorageSize() { return 0; }

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
size_t debugArgsStorageSize(const T& arg, const Args&... args) {
  return debugArgStorageSize(arg) + debugArgsStorageSize(args...);
}

}  // namespace details
}  // namespace inspector