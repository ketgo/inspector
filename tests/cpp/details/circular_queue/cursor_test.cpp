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

#include <inspector/details/circular_queue/cursor.hpp>

using namespace inspector::details;

namespace {

constexpr auto kBufferSize = 10;

using Cursor = circular_queue::Cursor<kBufferSize>;
using AtomicCursor = circular_queue::AtomicCursor<kBufferSize>;

}  // namespace

TEST(CursorTestFixture, TestAddOperation) {
  Cursor cursor(false, kBufferSize - 1);

  auto new_cursor = cursor + 5;
  ASSERT_TRUE(new_cursor.Overflow());
  ASSERT_EQ(new_cursor.Location(), 4);
}

TEST(CursorTestFixture, TestAtomicCursorIsLockFree) {
  ASSERT_TRUE(AtomicCursor().is_lock_free());
}

TEST(CursorTestFixture, TestIsBehind) {
  Cursor cursor(false, 2);
  ASSERT_TRUE(cursor < cursor + (kBufferSize - 1));
  ASSERT_FALSE(cursor < cursor + kBufferSize);
  ASSERT_FALSE(cursor < cursor + (kBufferSize + 1));
}
