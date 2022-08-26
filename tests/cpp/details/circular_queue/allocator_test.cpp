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

#include <thread>

#include <inspector/details/circular_queue/allocator.hpp>

using namespace inspector::details;

namespace {
constexpr auto kMaxAttempts = 32;
constexpr auto kBufferSize = 128;
constexpr auto kMaxProducers = 2;
constexpr auto kMaxConsumers = 2;
constexpr auto kCursorTimeoutNs = 200000UL;  // 0.2 ms
constexpr auto kStartMark = 924926508;

using Allocator =
    circular_queue::Allocator<char, kBufferSize, kMaxProducers, kMaxConsumers>;
using MemoryBlock = circular_queue::MemoryBlock<char>;
using WriteMemoryBlockHandle = circular_queue::MemoryBlockHandle<char>;
using ReadMemoryBlockHandle = circular_queue::MemoryBlockHandle<const char>;

}  // namespace

TEST(AllocatorTestFixture, TestProcessingOfStaleMemoryBlock) {
  Allocator allocator(kCursorTimeoutNs, kStartMark);

  {
    WriteMemoryBlockHandle handle = allocator.Allocate(1, kMaxAttempts);
    ASSERT_TRUE(handle);
    handle[0] = 'a';
  }

  WriteMemoryBlockHandle stale_handle = allocator.Allocate(1, kMaxAttempts);
  // Removing start marker for testing
  auto block =
      reinterpret_cast<MemoryBlock*>(stale_handle.Data() - sizeof(MemoryBlock));
  block->start_marker = 0;
  std::this_thread::sleep_for(
      std::chrono::nanoseconds{kCursorTimeoutNs + 1000});

  ReadMemoryBlockHandle handle = allocator.Allocate(kMaxAttempts);
  ASSERT_TRUE(handle);
  ASSERT_EQ(handle[0], 'a');

  handle = allocator.Allocate(kMaxAttempts);
  ASSERT_FALSE(stale_handle.IsValid());
  ASSERT_FALSE(handle);
}