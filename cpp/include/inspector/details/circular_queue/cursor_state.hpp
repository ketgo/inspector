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

namespace inspector {
namespace details {
namespace circular_queue {

// ============================================================================

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
  CursorState(const bool allocated_, const uint64_t timestamp_);
};

// -----------------------------
// CursorState Implementation
// -----------------------------

inline CursorState::CursorState(const bool allocated_,
                                const uint64_t timestamp_)
    : allocated(allocated_), timestamp(timestamp_) {}

// ============================================================================

/**
 * @brief Lock free atomic cursor state.
 *
 */
using AtomicCursorState = std::atomic<CursorState>;

// ============================================================================

}  // namespace circular_queue
}  // namespace details
}  // namespace inspector