/**
 * Copyright 2022 Ketan Goyal
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

#include <type_traits>

namespace inspector {
namespace details {
namespace circular_queue {

// ============================================================================

/**
 * @brief Data structure representing a memory block in the circular queue.
 *
 * @tparam T Type of objects stored in the memory block.
 */
template <class T>
struct __attribute__((packed)) MemoryBlock {
  // Ensures at compile time that the parameter T has trivial memory layout.
  static_assert(std::is_trivial<T>::value,
                "The data type used does not have a trivial memory layout.");

  uint32_t start_marker;
  std::size_t size;
  T data[0];
};

// ============================================================================

}  // namespace circular_queue
}  // namespace details
}  // namespace inspector
