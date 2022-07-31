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

#include "monitors/common/consumer.hpp"
#include <inspector/details/shared_object.hpp>

using namespace inspector;

namespace {
constexpr auto kBatchSize = 10;
}

TEST(ConsumerTestFixture, BasicConsumer) {
  Consumer consumer;

  // Consuming events from an empty queue
  ASSERT_TRUE(consumer.Consume(kBatchSize).empty());

  // Preparing queue by publishing a test event for testing.
  std::string test_event = "testing";
  auto queue = details::shared_object::Get<details::EventQueue>(
      details::kEventQueueSystemUniqueName);
  ASSERT_EQ(queue->Publish(test_event), details::EventQueue::Status::OK);

  // Consume events from queue.
  auto events = consumer.Consume(kBatchSize);
  ASSERT_EQ(events.size(), 1);
  ASSERT_EQ(events[0], test_event);
}

TEST(ConsumerTestFixture, PeriodicConsumer) {}