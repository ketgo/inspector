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

#include "tools/reader/priority_queue.h"

namespace {
constexpr auto kTestWindowSize = 50;
constexpr auto kWaitTimeout = std::chrono::microseconds{100};
}  // namespace

TEST(SlidingWindowPriorityQueueTestFixture, TestEmptyQueue) {
  inspector::SlidingWindowPriorityQueue<int> queue(kTestWindowSize);

  std::pair<int64_t, int> value;
  ASSERT_FALSE(queue.TryPop(value));
}

TEST(SlidingWindowPriorityQueueTestFixture, TestFullQueue) {
  inspector::SlidingWindowPriorityQueue<int> queue(kTestWindowSize);

  queue.Push({10, 1});
  queue.Push({20, 2});
  queue.Push({59, 3});
  ASSERT_FALSE(queue.Push({60, 4}, kWaitTimeout));
  ASSERT_FALSE(queue.TryPush({60, 4}));
}

TEST(SlidingWindowPriorityQueueTestFixture, TestForElementsWithinWindow) {
  inspector::SlidingWindowPriorityQueue<int> queue(kTestWindowSize);

  queue.Push({25, 1});
  queue.Push({10, 2});
  queue.Push({40, 3});

  auto top = queue.Top();
  ASSERT_EQ(top.first, 10);
  ASSERT_EQ(top.second, 2);

  std::pair<int64_t, int> value;
  queue.Pop(value);
  ASSERT_EQ(value.first, 10);
  ASSERT_EQ(value.second, 2);
  queue.Pop(value);
  ASSERT_EQ(value.first, 25);
  ASSERT_EQ(value.second, 1);
  queue.Pop(value);
  ASSERT_EQ(value.first, 40);
  ASSERT_EQ(value.second, 3);
  ASSERT_FALSE(queue.Pop(value, kWaitTimeout));
  ASSERT_FALSE(queue.TryPop(value));
}