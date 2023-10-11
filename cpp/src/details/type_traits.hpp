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

namespace inspector {
namespace details {

/**
 * @brief Type trait to get `DebugArg::Type` enum value for an object type `T`.
 *
 * @tparam T Type of object.
 */
template <class T>
struct TypeId {};

// Template specializations for the different supported data types.

template <>
struct TypeId<int16_t> {
  static constexpr auto value = DebugArg::Type::TYPE_INT16;
};

template <>
struct TypeId<int32_t> {
  static constexpr auto value = DebugArg::Type::TYPE_INT32;
};

template <>
struct TypeId<int64_t> {
  static constexpr auto value = DebugArg::Type::TYPE_INT64;
};

template <>
struct TypeId<uint8_t> {
  static constexpr auto value = DebugArg::Type::TYPE_UINT8;
};

template <>
struct TypeId<uint16_t> {
  static constexpr auto value = DebugArg::Type::TYPE_UINT16;
};

template <>
struct TypeId<uint32_t> {
  static constexpr auto value = DebugArg::Type::TYPE_UINT32;
};

template <>
struct TypeId<uint64_t> {
  static constexpr auto value = DebugArg::Type::TYPE_UINT64;
};

template <>
struct TypeId<float> {
  static constexpr auto value = DebugArg::Type::TYPE_FLOAT;
};

template <>
struct TypeId<double> {
  static constexpr auto value = DebugArg::Type::TYPE_DOUBLE;
};

template <>
struct TypeId<char> {
  static constexpr auto value = DebugArg::Type::TYPE_CHAR;
};

template <>
struct TypeId<const char*> {
  static constexpr auto value = DebugArg::Type::TYPE_STRING;
};

}  // namespace details
}  // namespace inspector