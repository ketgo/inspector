/**
 * Copyright 2023 Ketan Goyal
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

#include "tools/common/queues/priority_queue.hpp"

#include <gtest/gtest.h>

namespace {

constexpr auto kTestMinWindowSize = 10;
constexpr auto kTestMaxWindowSize = 50;

}  // namespace

using Queue = inspector::details::SlidingWindowPriorityQueue<int>;

TEST(SlidingWindowPriorityQueueTestFixture, TestEmptyQueue) {
  Queue queue(kTestMinWindowSize, kTestMaxWindowSize);

  ASSERT_EQ(queue.tryPop().first, Queue::Result::kEmpty);
}

TEST(SlidingWindowPriorityQueueTestFixture, TestFullQueue) {
  Queue queue(kTestMinWindowSize, kTestMaxWindowSize);

  ASSERT_EQ(queue.push({10, 1}), Queue::Result::kSuccess);
  ASSERT_EQ(queue.push({20, 2}), Queue::Result::kSuccess);
  ASSERT_EQ(queue.push({60, 3}), Queue::Result::kSuccess);
  ASSERT_EQ(queue.tryPush({70, 4}), Queue::Result::kFull);
}

TEST(SlidingWindowPriorityQueueTestFixture, TestForElementsWithinWindow) {
  Queue queue(kTestMinWindowSize, kTestMaxWindowSize);

  ASSERT_EQ(queue.push({1, 0}), Queue::Result::kSuccess);
  ASSERT_EQ(queue.push({25, 1}), Queue::Result::kSuccess);
  ASSERT_EQ(queue.push({10, 2}), Queue::Result::kSuccess);
  ASSERT_EQ(queue.push({40, 3}), Queue::Result::kSuccess);

  auto value = queue.pop();
  ASSERT_EQ(value.first, Queue::Result::kSuccess);
  ASSERT_EQ(value.second.first, 1);
  ASSERT_EQ(value.second.second, 0);
  value = queue.pop();
  ASSERT_EQ(value.first, Queue::Result::kSuccess);
  ASSERT_EQ(value.second.first, 10);
  ASSERT_EQ(value.second.second, 2);
  value = queue.pop();
  ASSERT_EQ(value.first, Queue::Result::kSuccess);
  ASSERT_EQ(value.second.first, 25);
  ASSERT_EQ(value.second.second, 1);
  value = queue.pop();
  ASSERT_EQ(value.first, Queue::Result::kSuccess);
  ASSERT_EQ(value.second.first, 40);
  ASSERT_EQ(value.second.second, 3);
  ASSERT_EQ(queue.tryPop().first, Queue::Result::kEmpty);
}