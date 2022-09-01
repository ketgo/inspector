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

#include <memory>

#include <inspector/details/system.hpp>

#include "tools/reader/basic_reader.hpp"

using namespace inspector;

namespace {
constexpr auto kMaxAttempt = 32;
constexpr auto kEventQueueName = "inspector-basic_reader-test";
}  // namespace

class BasicReaderTestFixture : public ::testing::Test {
 protected:
  static details::EventQueue* queue_;
  std::shared_ptr<BasicReader> reader_;

  static void SetUpTestSuite() {
    queue_ = details::system::CreateSharedObject<details::EventQueue>(
        kEventQueueName);
  }

  static void TearDownTestSuite() {
    details::system::RemoveSharedObject(kEventQueueName);
  }

  void SetUp() override {
    reader_ = std::make_shared<BasicReader>(*queue_, kMaxAttempt);
  }
  void TearDown() override {}
};

details::EventQueue* BasicReaderTestFixture::queue_;

TEST_F(BasicReaderTestFixture, IterateEmptyQueue) {
  std::vector<std::string> events;
  for (auto&& event : *reader_) {
    events.push_back(std::move(event));
  }

  ASSERT_TRUE(events.empty());
}

TEST_F(BasicReaderTestFixture, IterateNonEmptyQueue) {
  // Preparing queue by publishing a test event for testing.
  std::string test_event_1 = "testing_1";
  ASSERT_EQ(queue_->Publish(test_event_1), details::EventQueue::Status::OK);
  std::string test_event_2 = "testing_2";
  ASSERT_EQ(queue_->Publish(test_event_2), details::EventQueue::Status::OK);

  std::vector<std::string> events;
  for (auto&& event : *reader_) {
    events.push_back(std::move(event));
  }

  ASSERT_EQ(events.size(), 2);
  ASSERT_EQ(events[0], test_event_1);
  ASSERT_EQ(events[1], test_event_2);
}