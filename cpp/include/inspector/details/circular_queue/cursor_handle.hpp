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

#include <inspector/details/circular_queue/cursor.hpp>
#include <inspector/details/circular_queue/cursor_state.hpp>

namespace inspector {
namespace details {
namespace circular_queue {

// ============================================================================

/**
 * @brief The class `CursorHandle` exposes an atomic cursor and provides a
 * convenient RAII way for managing its state.
 *
 * @note The class does not satisfy CopyConstructable and CopyAssignable
 * concepts. However, it does satisfy MoveConstructable and MoveAssignable
 * concepts.
 *
 * @tparam CursorPool The type of cursor pool.
 */
template <class CursorPool>
class CursorHandle {
 public:
  CursorHandle(const CursorHandle &other) = delete;
  CursorHandle &operator=(const CursorHandle &other) = delete;

  /**
   * @brief Construct a new Cursor Handle object.
   *
   */
  CursorHandle();

  /**
   * @brief Construct a new Cursor Handle object.
   *
   * @param cursor Reference to the cursor.
   * @param pool Reference to the cursor pool.
   */
  CursorHandle(AtomicCursor &cursor, CursorPool &pool);

  /**
   * @brief Construct a new cursor handle object.
   *
   * @param other Rvalue reference to other handle.
   */
  CursorHandle(CursorHandle &&other);

  /**
   * @brief Move assign cursor handle.
   *
   * @param other Rvalue reference to other handle.
   * @returns Reference to the handle.
   */
  CursorHandle &operator=(CursorHandle &&other);

  /**
   * @brief Dereference operators
   *
   */
  AtomicCursor &operator*() const;

  /**
   * @brief Reference operator
   *
   */
  AtomicCursor *operator->() const;

  /**
   * @brief Check if handle is valid.
   *
   */
  operator bool() const;

  /**
   * @brief Destroy the Cursor Handle object.
   *
   */
  ~CursorHandle();

 private:
  /**
   * @brief Release the allocated cursor back to the pool.
   *
   */
  void Release();

  AtomicCursor *cursor_;
  CursorPool *pool_;
};

// ------------------------------------
// CursorHandle Implementation
// ------------------------------------

template <class CursorPool>
void CursorHandle<CursorPool>::Release() {
  if (pool_) {
    pool_->Release(cursor_);
  }
}

// ------------- public ---------------

template <class CursorPool>
CursorHandle<CursorPool>::CursorHandle() : cursor_(nullptr), pool_(nullptr) {}

template <class CursorPool>
CursorHandle<CursorPool>::CursorHandle(AtomicCursor &cursor, CursorPool &pool)
    : cursor_(std::addressof(cursor)), pool_(std::addressof(pool)) {}

template <class CursorPool>
CursorHandle<CursorPool>::CursorHandle(CursorHandle &&other)
    : cursor_(other.cursor_), pool_(other.pool_) {
  other.cursor_ = nullptr;
  other.pool_ = nullptr;
}

template <class CursorPool>
CursorHandle<CursorPool> &CursorHandle<CursorPool>::operator=(
    CursorHandle &&other) {
  if (this != &other) {
    Release();
    cursor_ = other.cursor_;
    pool_ = other.pool_;
    other.cursor_ = nullptr;
    other.pool_ = nullptr;
  }
  return *this;
}

template <class CursorPool>
AtomicCursor &CursorHandle<CursorPool>::operator*() const {
  return *cursor_;
}

template <class CursorPool>
AtomicCursor *CursorHandle<CursorPool>::operator->() const {
  return cursor_;
}

template <class CursorPool>
CursorHandle<CursorPool>::operator bool() const {
  return cursor_ != nullptr && pool_ != nullptr;
}

template <class CursorPool>
CursorHandle<CursorPool>::~CursorHandle() {
  Release();
}

// ============================================================================

}  // namespace circular_queue
}  // namespace details
}  // namespace inspector