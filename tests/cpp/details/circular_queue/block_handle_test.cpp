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

#include <array>

#include <inspector/details/circular_queue/block_handle.hpp>

using namespace inspector::details;

namespace {

using AtomicCursor = circular_queue::AtomicCursor;
using CursorState = circular_queue::CursorState;
using AtomicCursorState = circular_queue::AtomicCursorState;
using CursorHandle = circular_queue::CursorHandle;
using MemoryBlock = circular_queue::MemoryBlock<char>;
using MemoryBlockHandle = circular_queue::MemoryBlockHandle<char>;

constexpr auto kBufferSize = sizeof(MemoryBlock) + 10;

}  // namespace

TEST(MemoryBlockHandleTestFixture, TestConstructor) {
  AtomicCursor cursor;
  AtomicCursorState cursor_state;
  CursorState reserved_state(true, 40254);
  cursor_state.store(reserved_state);
  CursorHandle cursor_handle(cursor, cursor_state);
  std::array<char, kBufferSize> buffer;
  auto *block = reinterpret_cast<MemoryBlock *>(buffer.data());
  block->start_marker = circular_queue::kStartMarker;
  block->size = 10;

  {
    MemoryBlockHandle handle(*block, std::move(cursor_handle));

    ASSERT_EQ(handle.Data(), block->data);
    ASSERT_EQ(handle.Size(), block->size);
  }

  CursorState released_state(false, 0);
  ASSERT_EQ(cursor_state.load(), released_state);
}