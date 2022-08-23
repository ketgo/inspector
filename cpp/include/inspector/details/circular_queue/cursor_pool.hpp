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
#include <inspector/details/circular_queue/cursor.hpp>

namespace inspector {
namespace details {
namespace circular_queue {

// ============================================================================

/**
 * @brief A lock-free and wait-free pool of cursors used by the circular queue
 * for read/write operations. The pool also contains the head cursor which
 * contains the read/write head location.
 *
 * @tparam POOL_SIZE Number of cursors in the pool.
 */
template <std::size_t POOL_SIZE>
class CursorPool {
  friend class CursorHandle<CursorPool>;

  /**
   * @brief The data structure `CursorState` stores the state information of a
   * cursor. This includes two values, a flag indicating if the cursor is free,
   * and the timestamp when the cursor was last allocated.
   *
   * @note The structure is restricted to 64 bits in order to make the atomic
   * type `AtomicCursorState` lock free.
   */
  struct CursorState {
    bool allocated : 1;
    uint64_t timestamp : 63;

    CursorState() = default;
    CursorState(const bool allocated_, const uint64_t timestamp_)
        : allocated(allocated_), timestamp(timestamp_) {}
  };
  using AtomicCursorState = std::atomic<CursorState>;

 public:
  /**
   * @brief Construct a new CursorPool object.
   *
   */
  CursorPool();

  /**
   * @brief Check if the given cursor is behind all the allocated cursors in the
   * pool.
   *
   * @returns `true` if behind else `false`.
   */
  bool IsBehind(const Cursor &cursor) const;

  /**
   * @brief Check if the given cursor is ahead of all the allocated cursors in
   * the pool.
   *
   * @returns `true` if ahead else `false`.
   */
  bool IsAhead(const Cursor &cursor) const;

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
  CursorHandle<CursorPool> Allocate(std::size_t max_attempt);

 private:
  /**
   * @brief Release an allocated cursor.
   *
   * @param cursor Pointer to the cursor.
   */
  void Release(AtomicCursor *cursor);

  AtomicCursorState cursor_state_[POOL_SIZE];
  AtomicCursor cursor_[POOL_SIZE];
};

// -----------------------------
// CursorPool Implementation
// -----------------------------

template <std::size_t POOL_SIZE>
void CursorPool<POOL_SIZE>::Release(AtomicCursor *cursor) {
  assert(cursor != nullptr);
  CursorState cursor_state(false, 0);
  const std::size_t idx = cursor - cursor_;
  cursor_state_[idx].store(cursor_state, std::memory_order_seq_cst);
}

// ------- public --------------

template <std::size_t POOL_SIZE>
CursorPool<POOL_SIZE>::CursorPool() {
  // Initial cursor value
  Cursor cursor(false, 0);
  CursorState cursor_state(false, 0);
  for (std::size_t idx = 0; idx < POOL_SIZE; ++idx) {
    cursor_[idx].store(cursor, std::memory_order_seq_cst);
    cursor_state_[idx].store(cursor_state, std::memory_order_seq_cst);
  }
}

template <std::size_t POOL_SIZE>
bool CursorPool<POOL_SIZE>::IsBehind(const Cursor &cursor) const {
  for (std::size_t idx = 0; idx < POOL_SIZE; ++idx) {
    if (cursor_state_[idx].load(std::memory_order_seq_cst).allocated) {
      auto _cursor = cursor_[idx].load(std::memory_order_seq_cst);
      if (_cursor <= cursor) {
        return false;
      }
    }
  }
  return true;
}

template <std::size_t POOL_SIZE>
bool CursorPool<POOL_SIZE>::IsAhead(const Cursor &cursor) const {
  for (std::size_t idx = 0; idx < POOL_SIZE; ++idx) {
    if (cursor_state_[idx].load(std::memory_order_seq_cst).allocated) {
      auto _cursor = cursor_[idx].load(std::memory_order_seq_cst);
      if (cursor <= _cursor) {
        return false;
      }
    }
  }
  return true;
}

template <std::size_t POOL_SIZE>
CursorHandle<CursorPool<POOL_SIZE>> CursorPool<POOL_SIZE>::Allocate(
    std::size_t max_attempt) {
  while (max_attempt) {
    const std::size_t idx = Random() % POOL_SIZE;
    CursorState expected(false, 0);
    CursorState desired(
        true, std::chrono::system_clock::now().time_since_epoch().count());
    if (cursor_state_[idx].compare_exchange_strong(expected, desired)) {
      return {cursor_[idx], *this};
    }
    // TODO: Check if the cursor is stale
    --max_attempt;
  }
  return {};
}

// ============================================================================

}  // namespace circular_queue
}  // namespace details
}  // namespace inspector
