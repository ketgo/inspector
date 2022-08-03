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

#include <inspector/reader.hpp>

using namespace inspector;

namespace {
constexpr auto kMaxAttempt = 32;
}

TEST(ReaderTestFixture, IterateEmptyQueue) {
  Config config;
  config.max_attempt = kMaxAttempt;
  config.remove = true;
  Reader::SetConfig(config);
  Reader reader;
  std::vector<std::string> events;
  for (auto&& event : reader) {
    events.push_back(std::move(event));
  }

  ASSERT_TRUE(events.empty());
}

TEST(ReaderTestFixture, IterateNonEmptyQueue) {
  Config config;
  config.max_attempt = kMaxAttempt;
  config.remove = true;
  Reader::SetConfig(config);

  // Preparing queue by publishing a test event for testing.
  auto queue = details::shared_object::GetOrCreate<details::EventQueue>(
      config.queue_system_unique_name);
  std::string test_event_1 = "testing_1";
  ASSERT_EQ(queue->Publish(test_event_1), details::EventQueue::Status::OK);
  std::string test_event_2 = "testing_2";
  ASSERT_EQ(queue->Publish(test_event_2), details::EventQueue::Status::OK);

  Reader reader;
  std::vector<std::string> events;
  for (auto&& event : reader) {
    events.push_back(std::move(event));
  }

  ASSERT_EQ(events.size(), 2);
  ASSERT_EQ(events[0], test_event_1);
  ASSERT_EQ(events[1], test_event_2);
}