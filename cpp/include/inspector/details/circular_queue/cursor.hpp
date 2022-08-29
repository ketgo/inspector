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

#include <atomic>
#include <cassert>

namespace inspector {
namespace details {
namespace circular_queue {

// ============================================================================

/**
 * @brief Circular queue cursor.
 *
 * The cursor represents a location to an object inside the circular queue
 * buffer. A 64 bit data structure is used to store the following values:
 *
 *  a. [Bit 0]: Overflow sign of the cursor. The overflow sign is used to check
 *              for the overflow phase. Upon overflow the sign is flipped.
 *  b. [Bits 1-63]: Location value of the cursor used to get the index of an
 *                  object in the circular queue.
 *
 * @note The cursor is restricted to 64 bits in order to make the atomic type
 * `AtomicCursor` lock free.
 *
 * @tparam BUFFER_SIZE Size of the circular queue buffer.
 */
template <std::size_t BUFFER_SIZE>
class Cursor {
 public:
  /**
   * @brief Construct a new Cursor object.
   *
   */
  Cursor() = default;

  /**
   * @brief Construct a new Cursor object.
   *
   * @param overflow Overflow sign.
   * @param location Cursor location.
   */
  Cursor(const bool overflow, const std::size_t location);

  /**
   * @brief Get the overflow sign of the cursor.
   *
   * @returns Boolean value representing the overflow sign of the cursor.
   */
  bool Overflow() const;

  /**
   * @brief Get the location value stored in the cursor.
   *
   * @returns The location value.
   */
  uint64_t Location() const;

  /**
   * @brief Operator to check if the given cursor is ahead to this cursor.
   *
   * @param cursor Constant reference to the cursor.
   * @return `true` if ahead else `false`.
   */
  bool operator<(const Cursor &cursor) const;

  /**
   * @brief Operator to check if the given cursor is ahead or equal to this
   * cursor.
   *
   * @param cursor Constant reference to the cursor.
   * @return `true` if ahead or equal else `false`.
   */
  bool operator<=(const Cursor &cursor) const;

  /**
   * @brief Operator to check if the given cursor points to the same location.
   *
   * @param cursor Constant reference to the cursor.
   * @returns `true` if same location else `false`.
   */
  bool operator==(const Cursor &cursor) const;

  /**
   * @brief Operator to check if the given cursor does not point to the same
   * location.
   *
   * @param cursor Constant reference to the cursor.
   * @returns `true` if not same location else `false`.
   */
  bool operator!=(const Cursor &cursor) const;

  /**
   * @brief Add a value to the location stored in the cursor to get a new
   * cursor.
   *
   * @param offset Value to add.
   * @returns Cursor containing updated location value.
   */
  Cursor operator+(const std::size_t value) const;

  /**
   * @brief Update the location stored in the cursor by adding the given value.
   *
   * @param offset Value to add.
   * @returns Reference to the updated cursor.
   */
  Cursor &operator+=(const std::size_t value);

 private:
  /**
   * @brief Flip the overflow sign of the cursor.
   *
   */
  void FlipOverflow();

  bool overflow_ : 1;
  uint64_t location_ : 63;
};

// ---------------------------------------
// Cursor Implementation
// ---------------------------------------

template <std::size_t BUFFER_SIZE>
void Cursor<BUFFER_SIZE>::FlipOverflow() {
  overflow_ = !overflow_;
}

// ---------- public ----------------

template <std::size_t BUFFER_SIZE>
Cursor<BUFFER_SIZE>::Cursor(const bool overflow, const std::size_t location)
    : overflow_(overflow), location_(location) {
  assert(location < BUFFER_SIZE);
}

template <std::size_t BUFFER_SIZE>
bool Cursor<BUFFER_SIZE>::Overflow() const {
  return overflow_;
}

template <std::size_t BUFFER_SIZE>
uint64_t Cursor<BUFFER_SIZE>::Location() const {
  return location_;
}

template <std::size_t BUFFER_SIZE>
bool Cursor<BUFFER_SIZE>::operator<(const Cursor &cursor) const {
  return overflow_ == cursor.overflow_ ? location_ < cursor.location_
                                       : location_ > cursor.location_;
}

template <std::size_t BUFFER_SIZE>
bool Cursor<BUFFER_SIZE>::operator<=(const Cursor &cursor) const {
  return overflow_ == cursor.overflow_ ? location_ <= cursor.location_
                                       : location_ > cursor.location_;
}

template <std::size_t BUFFER_SIZE>
bool Cursor<BUFFER_SIZE>::operator==(const Cursor &cursor) const {
  return overflow_ == cursor.overflow_ && location_ <= cursor.location_;
}

template <std::size_t BUFFER_SIZE>
bool Cursor<BUFFER_SIZE>::operator!=(const Cursor &cursor) const {
  return !(*this == cursor);
}

template <std::size_t BUFFER_SIZE>
Cursor<BUFFER_SIZE> Cursor<BUFFER_SIZE>::operator+(
    const std::size_t value) const {
  Cursor rvalue(overflow_, location_);
  rvalue += value;
  return rvalue;
}

template <std::size_t BUFFER_SIZE>
Cursor<BUFFER_SIZE> &Cursor<BUFFER_SIZE>::operator+=(const std::size_t value) {
  if ((location_ + value) / BUFFER_SIZE) {
    FlipOverflow();
  }
  location_ = (location_ + value) % BUFFER_SIZE;
  return *this;
}

// ============================================================================

/**
 * @brief Lock free atomic cursor.
 *
 */
template <std::size_t BUFFER_SIZE>
using AtomicCursor = std::atomic<Cursor<BUFFER_SIZE>>;

// ============================================================================

}  // namespace circular_queue
}  // namespace details
}  // namespace inspector
