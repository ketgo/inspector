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

TEST(ConsumerTestFixture, Consume) {
  Consumer consumer;

  // Consuming message from empty queue
  ASSERT_TRUE(consumer.Consume().empty());

  // Publishing test message
  std::string test_message = "testing";
  auto queue = details::shared_object::Get<details::EventQueue>(
      details::kTraceQueueSystemUniqueName);
  ASSERT_EQ(queue->Publish(test_message), details::EventQueue::Result::SUCCESS);
  // Consume message
  ASSERT_EQ(consumer.Consume(), test_message);
}