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

#include <inspector/details/circular_queue/block.hpp>
#include <inspector/details/circular_queue/cursor_handle.hpp>

namespace inspector {
namespace details {
namespace circular_queue {

// ============================================================================

/**
 * @brief Handle to an allocated memory block in the circular queue. It exposes
 * the allocated memory for reading or writing.
 *
 * @note The class does not satisfy CopyConstructable and CopyAssignable
 * concepts. However, it does satisfy MoveConstructable and MoveAssignable
 * concepts.
 *
 * @tparam T Type of objects stored in the memory block.
 * @tparam BUFFER_SIZE Size of the circular queue buffer.
 */
template <class T, std::size_t BUFFER_SIZE>
class MemoryBlockHandle {
 public:
  /**
   * @brief Construct a new Memory Block Handle object.
   *
   */
  MemoryBlockHandle();

  /**
   * @brief Construct a new Memory Block Handle object.
   *
   * @param block Reference to the memory block.
   * @param handle Rvalue reference to the cursor handle.
   */
  MemoryBlockHandle(MemoryBlock<T>& block, CursorHandle<BUFFER_SIZE>&& handle);

  /**
   * @brief Get the number of objects of type T stored in the memory block.
   *
   */
  std::size_t Size() const;

  /**
   * @brief Get the pointer to the first T type object in the memory block.
   *
   */
  T* Data() const;

  /**
   * @brief Get object at the given index in the memory block.
   *
   * @param n Index value.
   * @returns Reference to the object.
   */
  T& operator[](std::size_t n) const;

  /**
   * @brief Check if the handle is null.
   *
   */
  operator bool() const;

  /**
   * @brief Check if the handle is valid.
   *
   * Along with the null check, this method also checks the validity of the
   * underlying cursor state.
   *
   * @return `true` if handle is valid else `false`.
   */
  bool IsValid() const;

 private:
  MemoryBlock<T>* block_;
  CursorHandle<BUFFER_SIZE> handle_;
};

// --------------------------------
// MemoryBlockHandle Implementation
// --------------------------------

template <class T, std::size_t BUFFER_SIZE>
MemoryBlockHandle<T, BUFFER_SIZE>::MemoryBlockHandle()
    : block_(nullptr), handle_() {}

template <class T, std::size_t BUFFER_SIZE>
MemoryBlockHandle<T, BUFFER_SIZE>::MemoryBlockHandle(
    MemoryBlock<T>& block, CursorHandle<BUFFER_SIZE>&& handle)
    : block_(std::addressof(block)), handle_(std::move(handle)) {}

template <class T, std::size_t BUFFER_SIZE>
std::size_t MemoryBlockHandle<T, BUFFER_SIZE>::Size() const {
  return block_->size;
}

template <class T, std::size_t BUFFER_SIZE>
T* MemoryBlockHandle<T, BUFFER_SIZE>::Data() const {
  return block_->data;
}

template <class T, std::size_t BUFFER_SIZE>
T& MemoryBlockHandle<T, BUFFER_SIZE>::operator[](std::size_t n) const {
  assert(n < block_->size);
  return block_->data[n];
}

template <class T, std::size_t BUFFER_SIZE>
MemoryBlockHandle<T, BUFFER_SIZE>::operator bool() const {
  return block_ != nullptr && bool(handle_);
}

template <class T, std::size_t BUFFER_SIZE>
bool MemoryBlockHandle<T, BUFFER_SIZE>::IsValid() const {
  if (handle_.IsValid()) {
    return block_ != nullptr;
  }
  return false;
}

// ============================================================================

}  // namespace circular_queue
}  // namespace details
}  // namespace inspector