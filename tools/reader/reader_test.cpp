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
#include <inspector/details/trace_event.hpp>

#include "tools/reader/reader.hpp"

using namespace inspector;

namespace {
constexpr auto kEventQueueName = "inspector-reader-test";
constexpr auto kReadTimeout = std::chrono::microseconds{100};
}  // namespace

class ReaderTestFixture : public ::testing::Test {
 protected:
  static details::EventQueue* queue_;
  std::shared_ptr<Reader> reader_;

  static void SetUpTestSuite() {
    queue_ = details::system::CreateSharedObject<details::EventQueue>(
        kEventQueueName);
  }

  static void TearDownTestSuite() {
    details::system::RemoveSharedObject(kEventQueueName);
  }

  void SetUp() override {
    reader_ = std::make_shared<Reader>(kReadTimeout, kEventQueueName);
  }
  void TearDown() override {}
};

details::EventQueue* ReaderTestFixture::queue_;

TEST_F(ReaderTestFixture, IterateEmptyQueue) {
  std::vector<Event> events;
  for (auto&& event : *reader_) {
    events.push_back(std::move(event));
  }

  ASSERT_TRUE(events.empty());
}

TEST_F(ReaderTestFixture, IterateNonEmptyQueue) {
  // Preparing queue by publishing a test event for testing.
  details::TraceEvent test_event_1('T', "TestTraceEvent1");
  ASSERT_EQ(queue_->Publish(test_event_1.String()),
            details::EventQueue::Status::OK);
  details::TraceEvent test_event_2('T', "TestTraceEvent2");
  ASSERT_EQ(queue_->Publish(test_event_2.String()),
            details::EventQueue::Status::OK);

  std::vector<Event> events;
  for (auto&& event : *reader_) {
    events.push_back(std::move(event));
  }

  ASSERT_EQ(events.size(), 2);
  ASSERT_EQ(events[0].Type(), 0);
  ASSERT_NE(events[0].Timestamp(), 0);
  ASSERT_EQ(events[0].Pid(), details::system::GetProcessId());
  ASSERT_EQ(events[0].Tid(), details::system::GetThreadId());
  ASSERT_EQ(events[0].Payload(), "T|TestTraceEvent1");
  ASSERT_EQ(events[1].Type(), 0);
  ASSERT_NE(events[1].Timestamp(), 0);
  ASSERT_EQ(events[1].Pid(), details::system::GetProcessId());
  ASSERT_EQ(events[1].Tid(), details::system::GetThreadId());
  ASSERT_EQ(events[1].Payload(), "T|TestTraceEvent2");
}