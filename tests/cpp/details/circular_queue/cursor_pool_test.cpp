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
#include <unordered_set>

#include "utils/random.hpp"
#include "utils/threads.hpp"

#include <inspector/details/circular_queue/cursor_pool.hpp>

using namespace inspector::details;

namespace {

constexpr auto kThreadCount = 10;
constexpr auto kPoolSize = 10;
constexpr auto kMaxAttempts = 32;
constexpr auto kBufferSize = 124;
constexpr auto kCursorTimeoutNs = 200000UL;  // 0.2 ms

using Cursor = circular_queue::Cursor<kBufferSize>;
using AtomicCursor = circular_queue::AtomicCursor<kBufferSize>;
using CursorPool = circular_queue::CursorPool<kPoolSize, kBufferSize>;
using CursorHandle = circular_queue::CursorHandle<kBufferSize>;

// AtomicCursor hasher
class AtomicCursorHash {
 public:
  std::size_t operator()(const CursorHandle& handle) const {
    return hash_(&(*handle));
  }

 private:
  std::hash<AtomicCursor*> hash_;
};

// Allocate method run from different threads
void Allocate(CursorPool& pool, CursorHandle& handle) {
  handle = pool.Allocate(kMaxAttempts);
}

}  // namespace

TEST(CircularQueueCursorPoolTestFixture, TestAllocateSingleThread) {
  CursorPool pool(kCursorTimeoutNs);

  std::array<CursorHandle, kThreadCount> handles;
  auto null_count = 0;
  std::unordered_set<AtomicCursor*> unique_cursors;
  for (size_t i = 0; i < kThreadCount; ++i) {
    handles[i] = pool.Allocate(kMaxAttempts);
    if (!handles[i]) {
      ++null_count;
    } else {
      unique_cursors.insert(&(*handles[i]));
    }
  }
  ASSERT_EQ(unique_cursors.size(), kThreadCount - null_count);
}

TEST(CircularQueueCursorPoolTestFixture, TestAllocateMultipleThread) {
  CursorPool pool(kCursorTimeoutNs);

  std::array<CursorHandle, kThreadCount> handles;
  utils::Threads threads(kThreadCount);
  for (size_t i = 0; i < kThreadCount; ++i) {
    threads[i] = std::thread(&Allocate, std::ref(pool), std::ref(handles[i]));
  }
  threads.Wait();

  // Tracking null handles
  auto null_count = 0;
  std::unordered_set<AtomicCursor*> unique_cursors;
  for (auto& handle : handles) {
    if (!handle) {
      ++null_count;
    } else {
      unique_cursors.insert(&(*handle));
    }
  }
  ASSERT_EQ(unique_cursors.size(), kThreadCount - null_count);
}

TEST(CircularQueueCursorPoolTestFixture, TestIsBehind) {
  utils::RandomNumberGenerator<size_t> rand(kBufferSize - 10, kBufferSize + 10);
  Cursor start(false, 0);
  size_t min = std::numeric_limits<size_t>::max();
  size_t max = std::numeric_limits<size_t>::min();
  std::array<CursorHandle, kThreadCount> handles;
  CursorPool pool(kCursorTimeoutNs);

  for (size_t i = 0; i < kThreadCount; ++i) {
    handles[i] = pool.Allocate(kMaxAttempts);
    if (handles[i]) {
      auto location = rand();
      handles[i]->store(start + location);
      min = min > location ? location : min;
      max = max < location ? location : max;
    }
  }

  ASSERT_TRUE(pool.IsBehind(start + (min - 5)));
  ASSERT_FALSE(pool.IsBehind(start + min));
  ASSERT_FALSE(pool.IsBehind(start + (min + max) / 2));
  ASSERT_FALSE(pool.IsBehind(start + max));
  ASSERT_FALSE(pool.IsBehind(start + (max + 5)));
}

TEST(CircularQueueCursorPoolTestFixture, TestIsBehindWithStaleCursor) {
  std::array<CursorHandle, 2> handles;
  CursorPool pool(kCursorTimeoutNs);

  // Creating stale cursor
  handles[0] = pool.Allocate(kMaxAttempts);
  handles[0]->store({false, 5}, std::memory_order_seq_cst);
  // Wait more than timeout to make the cursor stale
  std::this_thread::sleep_for(std::chrono::microseconds{kCursorTimeoutNs});

  handles[1] = pool.Allocate(kMaxAttempts);
  handles[1]->store({false, 10}, std::memory_order_seq_cst);

  ASSERT_TRUE(pool.IsBehind({false, 6}));
  ASSERT_FALSE(handles[0].IsValid());  // Stale cursor was released
  ASSERT_FALSE(pool.IsBehind({false, 12}));
}

TEST(CircularQueueCursorPoolTestFixture, TestIsAhead) {
  Cursor start(false, 0);
  utils::RandomNumberGenerator<size_t> rand(kBufferSize - 10, kBufferSize + 10);
  size_t min = std::numeric_limits<size_t>::max();
  size_t max = std::numeric_limits<size_t>::min();
  std::array<CursorHandle, kThreadCount> handles;
  CursorPool pool(kCursorTimeoutNs);

  for (size_t i = 0; i < kThreadCount; ++i) {
    handles[i] = pool.Allocate(kMaxAttempts);
    if (handles[i]) {
      auto location = rand();
      handles[i]->store(start + location);
      min = min > location ? location : min;
      max = max < location ? location : max;
    }
  }

  ASSERT_FALSE(pool.IsAhead(start + (min - 5)));
  ASSERT_FALSE(pool.IsAhead(start + min));
  ASSERT_FALSE(pool.IsAhead(start + (min + max) / 2));
  ASSERT_FALSE(pool.IsAhead(start + max));
  ASSERT_TRUE(pool.IsAhead(start + (max + 5)));
}

TEST(CircularQueueCursorPoolTestFixture, TestIsAheadWithStaleCursor) {
  std::array<CursorHandle, 2> handles;
  CursorPool pool(kCursorTimeoutNs);

  // Creating stale cursor
  handles[0] = pool.Allocate(kMaxAttempts);
  handles[0]->store({false, 10}, std::memory_order_seq_cst);
  // Wait more than timeout to make the cursor stale
  std::this_thread::sleep_for(std::chrono::microseconds{kCursorTimeoutNs});

  handles[1] = pool.Allocate(kMaxAttempts);
  handles[1]->store({false, 5}, std::memory_order_seq_cst);

  ASSERT_TRUE(pool.IsAhead({false, 8}));
  ASSERT_FALSE(handles[0].IsValid());  // Stale cursor was released
  ASSERT_FALSE(pool.IsAhead({false, 4}));
}
