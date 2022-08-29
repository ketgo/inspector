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

#include <gtest/gtest.h>

#include <inspector/details/circular_queue/cursor_handle.hpp>

using namespace inspector::details;

namespace {

constexpr auto kBufferSize = 10;

using AtomicCursor = circular_queue::AtomicCursor<kBufferSize>;
using CursorState = circular_queue::CursorState;
using AtomicCursorState = circular_queue::AtomicCursorState;
using CursorHandle = circular_queue::CursorHandle<kBufferSize>;

}  // namespace

TEST(CursorHandleTestFixture, TestRAIIRelease) {
  AtomicCursor cursor;
  AtomicCursorState state;
  CursorState reserved_state(true, 2422542);

  state.store(reserved_state);
  { CursorHandle handle(cursor, state, reserved_state); }
  auto released_state = state.load();
  ASSERT_EQ(released_state.allocated, false);
  ASSERT_EQ(released_state.timestamp, 0);
}

TEST(CursorHandleTestFixture, TestRAIIReleaseWhenCursorStateChanged) {
  AtomicCursor cursor;
  AtomicCursorState state;
  CursorState reserved_state(true, 2422542);

  state.store(reserved_state);
  {
    CursorHandle handle(cursor, state, reserved_state);
    state.store({true, 25820});
  }
  auto released_state = state.load();
  // Test that the cursor was not released since its state changed
  ASSERT_EQ(released_state.allocated, true);
  ASSERT_EQ(released_state.timestamp, 25820);
}

TEST(CursorHandleTestFixture, TestMoveConstructorAndMoveAssignment) {
  AtomicCursor cursor;
  AtomicCursorState state;
  CursorState reserved_state(true, 2422542);

  state.store(reserved_state);
  {
    CursorHandle handle_a(cursor, state, reserved_state);
    // Move constructor
    CursorHandle handle_b(std::move(handle_a));
    // Move assignment
    CursorHandle handle_c;
    handle_c = std::move(handle_b);
  }
  auto released_state = state.load();
  ASSERT_EQ(released_state.allocated, false);
  ASSERT_EQ(released_state.timestamp, 0);
}