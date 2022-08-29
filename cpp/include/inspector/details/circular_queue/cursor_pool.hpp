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

#include <chrono>

#include <inspector/details/random.hpp>
#include <inspector/details/circular_queue/cursor_handle.hpp>

namespace inspector {
namespace details {
namespace circular_queue {

// ============================================================================

/**
 * @brief A lock-free and wait-free pool of cursors used by the circular queue
 * for read/write operations.
 *
 * @tparam POOL_SIZE Number of cursors in the pool.
 * @tparam BUFFER_SIZE Size of the circular queue buffer.
 */
template <std::size_t POOL_SIZE, std::size_t BUFFER_SIZE>
class CursorPool {
 public:
  /**
   * @brief Construct a new CursorPool object.
   *
   * @param timeout_ns Cursor timeout in nano seconds.
   */
  CursorPool(const uint64_t timeout_ns);

  /**
   * @brief Check if the given cursor is behind all the allocated cursors in
   * the pool.
   *
   * @returns `true` if behind else `false`.
   */
  bool IsBehind(const Cursor<BUFFER_SIZE> &cursor) const;

  /**
   * @brief Check if the given cursor is ahead of all the allocated cursors in
   * the pool.
   *
   * @returns `true` if ahead else `false`.
   */
  bool IsAhead(const Cursor<BUFFER_SIZE> &cursor) const;

  /**
   * @brief Allocate a cursor from available set of free cursors.
   *
   * The method attempts to allocate a free cursor by random selection. It
   * performs `max_attempt` number of attempts. If no cursor is found then a
   * null pointer is returned.
   *
   * @param max_attempt Maximum number of attempts to perform.
   * @returns Pointer to the allocated cursor.
   */
  CursorHandle<BUFFER_SIZE> Allocate(std::size_t max_attempt);

 private:
  /**
   * @brief Check if the given state represents a stale cursor.
   *
   * @param cursor_state Constant reference to the cursor state.
   * @returns `true` if stale else `false`.
   */
  bool IsStale(const CursorState &cursor_state) const;

  mutable AtomicCursorState cursor_state_[POOL_SIZE];
  AtomicCursor<BUFFER_SIZE> cursor_[POOL_SIZE];
  const uint64_t timeout_ns_;
};

// -----------------------------
// CursorPool Implementation
// -----------------------------

template <std::size_t POOL_SIZE, std::size_t BUFFER_SIZE>
bool CursorPool<POOL_SIZE, BUFFER_SIZE>::IsStale(
    const CursorState &cursor_state) const {
  CursorState current(
      true, std::chrono::system_clock::now().time_since_epoch().count());
  if (timeout_ns_ < (current.timestamp - cursor_state.timestamp)) {
    return true;
  }
  return false;
}

// ------- public --------------

template <std::size_t POOL_SIZE, std::size_t BUFFER_SIZE>
CursorPool<POOL_SIZE, BUFFER_SIZE>::CursorPool(const uint64_t timeout_ns)
    : timeout_ns_(timeout_ns) {
  // Initial cursor value
  Cursor<BUFFER_SIZE> cursor(false, 0);
  CursorState cursor_state(false, 0);
  for (std::size_t idx = 0; idx < POOL_SIZE; ++idx) {
    cursor_[idx].store(cursor, std::memory_order_seq_cst);
    cursor_state_[idx].store(cursor_state, std::memory_order_seq_cst);
  }
}

template <std::size_t POOL_SIZE, std::size_t BUFFER_SIZE>
bool CursorPool<POOL_SIZE, BUFFER_SIZE>::IsBehind(
    const Cursor<BUFFER_SIZE> &cursor) const {
  for (std::size_t idx = 0; idx < POOL_SIZE; ++idx) {
    auto cursor_state = cursor_state_[idx].load(std::memory_order_seq_cst);
    if (cursor_state.allocated) {
      auto _cursor = cursor_[idx].load(std::memory_order_seq_cst);
      if (_cursor <= cursor) {
        if (IsStale(cursor_state)) {
          cursor_state_[idx].compare_exchange_strong(cursor_state, {false, 0},
                                                     std::memory_order_seq_cst);
          continue;
        }
        return false;
      }
    }
  }
  return true;
}

template <std::size_t POOL_SIZE, std::size_t BUFFER_SIZE>
bool CursorPool<POOL_SIZE, BUFFER_SIZE>::IsAhead(
    const Cursor<BUFFER_SIZE> &cursor) const {
  for (std::size_t idx = 0; idx < POOL_SIZE; ++idx) {
    auto cursor_state = cursor_state_[idx].load(std::memory_order_seq_cst);
    if (cursor_state.allocated) {
      auto _cursor = cursor_[idx].load(std::memory_order_seq_cst);
      if (cursor <= _cursor) {
        if (IsStale(cursor_state)) {
          cursor_state_[idx].compare_exchange_strong(cursor_state, {false, 0},
                                                     std::memory_order_seq_cst);
          continue;
        }
        return false;
      }
    }
  }
  return true;
}

template <std::size_t POOL_SIZE, std::size_t BUFFER_SIZE>
CursorHandle<BUFFER_SIZE> CursorPool<POOL_SIZE, BUFFER_SIZE>::Allocate(
    std::size_t max_attempt) {
  while (max_attempt) {
    const std::size_t idx = Random() % POOL_SIZE;
    CursorState expected(false, 0);
    CursorState desired(
        true, std::chrono::system_clock::now().time_since_epoch().count());
    if (cursor_state_[idx].compare_exchange_strong(expected, desired)) {
      return {cursor_[idx], cursor_state_[idx], desired};
    }
    --max_attempt;
  }
  return {};
}

// ============================================================================

}  // namespace circular_queue
}  // namespace details
}  // namespace inspector
