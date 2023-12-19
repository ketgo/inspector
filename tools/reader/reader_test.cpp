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

#include <inspector/config.hpp>
#include <inspector/trace.hpp>

#include "cpp/tests/testing.hpp"
#include "tools/reader/reader.hpp"

using namespace inspector;

namespace {
constexpr auto kEventQueueName = "inspector-reader-test";
}  // namespace

class ReaderTestFixture : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { Config::setEventQueueName(kEventQueueName); }
  static void TearDownTestSuite() { inspector::testing::removeEventQueue(); }
  void SetUp() override {}
  void TearDown() override { inspector::testing::emptyEventQueue(); }

  Reader reader_;
};

TEST_F(ReaderTestFixture, IterateEmptyQueue) {
  std::vector<TraceEvent> events;
  for (auto&& event : reader_) {
    events.push_back(std::move(event));
  }

  ASSERT_TRUE(events.empty());
}

TEST_F(ReaderTestFixture, IterateNonEmptyQueue) {
  // Preparing queue by publishing a test event for testing.
  syncBegin("test-event-1", 1);
  syncEnd("test-event-1");
  syncBegin("test-event-2", 2);
  syncEnd("test-event-2");

  std::vector<TraceEvent> events;
  for (auto&& event : reader_) {
    events.push_back(std::move(event));
  }

  ASSERT_EQ(events.size(), 4);
  ASSERT_EQ(events[0].type(),
            static_cast<event_type_t>(EventType::kSyncBeginTag));
  ASSERT_NE(events[0].timestampNs(), 0);
  ASSERT_EQ(events[0].pid(), details::getPID());
  ASSERT_EQ(events[0].tid(), details::getTID());
  ASSERT_EQ(std::string(events[0].name()), "test-event-1");
  ASSERT_EQ(events[1].type(),
            static_cast<event_type_t>(EventType::kSyncEndTag));
  ASSERT_NE(events[1].timestampNs(), 0);
  ASSERT_EQ(events[1].pid(), details::getPID());
  ASSERT_EQ(events[1].tid(), details::getTID());
  ASSERT_EQ(std::string(events[1].name()), "test-event-1");
  ASSERT_EQ(events[2].type(),
            static_cast<event_type_t>(EventType::kSyncBeginTag));
  ASSERT_NE(events[2].timestampNs(), 0);
  ASSERT_EQ(events[2].pid(), details::getPID());
  ASSERT_EQ(events[2].tid(), details::getTID());
  ASSERT_EQ(std::string(events[2].name()), "test-event-2");
  ASSERT_EQ(events[3].type(),
            static_cast<event_type_t>(EventType::kSyncEndTag));
  ASSERT_NE(events[3].timestampNs(), 0);
  ASSERT_EQ(events[3].pid(), details::getPID());
  ASSERT_EQ(events[3].tid(), details::getTID());
  ASSERT_EQ(std::string(events[3].name()), "test-event-2");
}