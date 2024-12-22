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

namespace inspector {
namespace tools {
namespace data {

using timestamp_t = int64_t;
using duration_t = uint64_t;

}  // namespace data
}  // namespace tools
}  // namespace inspector

/**
 * @brief Remove copy constructor.
 *
 */
#define NO_COPY_CTOR(T) T(const T& other) = delete

/**
 * @brief Remove move constructor.
 *
 */
#define NO_MOVE_CTOR(T) T(T&& other) = delete

/**
 * @brief Remove copy assignment.
 *
 */
#define NO_COPY_ASSIGN(T) T& operator=(const T& other) = delete

/**
 * @brief Remove move assignment.
 *
 */
#define NO_MOVE_ASSIGN(T) T& operator=(T&& other) = delete

/**
 * @brief Remove copy constructor and assignment.
 *
 */
#define NO_COPY(T)            \
  T(const T& other) = delete; \
  T& operator=(const T& other) = delete

/**
 * @brief Remove move constructor and assignment.
 *
 */
#define NO_MOVE(T)       \
  T(T&& other) = delete; \
  T& operator=(T&& other) = delete

/**
 * @brief Assign packed attribute to a data structure.
 *
 */
#define PACKED __attribute__((packed))