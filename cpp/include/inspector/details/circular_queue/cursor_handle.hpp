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
 */
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
   * @param cursor_state Reference to the cursor state.
   */
  CursorHandle(AtomicCursor &cursor, AtomicCursorState &cursor_state);

  /**
   * @brief Construct a new Cursor Handle object.
   *
   * @param cursor Reference to the cursor.
   * @param cursor_state Reference to the cursor state.
   * @param reserved_state Constant reference to the state of the cursor when it
   * was reserved.
   *
   * @note This CTOR avoids the need to perform an atomic load of the cursor
   * state.
   */
  CursorHandle(AtomicCursor &cursor, AtomicCursorState &cursor_state,
               const CursorState &reserved_state);

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
   * @brief Check if handle is not null.
   *
   */
  operator bool() const;

  /**
   * @brief Check if the handle is valid.
   *
   * Along with the null test, this method also checks for the validity of the
   * cursor state. The method is primarly used for testing but can be used
   * outside of testing as well.
   *
   * @returns `true` if valid else `false`.
   */
  bool IsValid() const;

  /**
   * @brief Destroy the Cursor Handle object.
   *
   */
  ~CursorHandle();

 private:
  /**
   * @brief Release the cursor managed by the handle.
   *
   */
  void Release();

  AtomicCursor *cursor_;
  AtomicCursorState *cursor_state_;
  CursorState reserved_state_;
};

// ------------------------------------
// CursorHandle Implementation
// ------------------------------------

inline void CursorHandle::Release() {
  if (cursor_state_) {
    // Set the cursor state to released only if its state has not changed. This
    // prevents a re-release of the same cursor which might lead to a data race.
    CursorState released_state(false, 0);
    cursor_state_->compare_exchange_strong(reserved_state_, released_state,
                                           std::memory_order_seq_cst);
  }
}

// ------------- public ---------------

inline CursorHandle::CursorHandle()
    : cursor_(nullptr), cursor_state_(nullptr), reserved_state_() {}

inline CursorHandle::CursorHandle(AtomicCursor &cursor,
                                  AtomicCursorState &cursor_state)
    : cursor_(std::addressof(cursor)),
      cursor_state_(std::addressof(cursor_state)),
      reserved_state_(cursor_state_->load(std::memory_order_seq_cst)) {}

inline CursorHandle::CursorHandle(AtomicCursor &cursor,
                                  AtomicCursorState &cursor_state,
                                  const CursorState &reserved_state)
    : cursor_(std::addressof(cursor)),
      cursor_state_(std::addressof(cursor_state)),
      reserved_state_(reserved_state) {}

inline CursorHandle::CursorHandle(CursorHandle &&other)
    : cursor_(other.cursor_),
      cursor_state_(other.cursor_state_),
      reserved_state_(other.reserved_state_) {
  other.cursor_ = nullptr;
  other.cursor_state_ = nullptr;
}

inline CursorHandle &CursorHandle::operator=(CursorHandle &&other) {
  if (this != &other) {
    Release();
    cursor_ = other.cursor_;
    cursor_state_ = other.cursor_state_;
    reserved_state_ = other.reserved_state_;
    other.cursor_ = nullptr;
    other.cursor_state_ = nullptr;
  }
  return *this;
}

inline AtomicCursor &CursorHandle::operator*() const { return *cursor_; }

inline AtomicCursor *CursorHandle::operator->() const { return cursor_; }

inline CursorHandle::operator bool() const {
  return cursor_ != nullptr && cursor_state_ != nullptr;
}

inline bool CursorHandle::IsValid() const {
  if (*this) {
    auto current_state = cursor_state_->load(std::memory_order_seq_cst);
    return current_state == reserved_state_;
  }
  return false;
}

inline CursorHandle::~CursorHandle() { Release(); }

// ============================================================================

}  // namespace circular_queue
}  // namespace details
}  // namespace inspector