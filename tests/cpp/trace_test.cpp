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

#include <inspector/trace.hpp>
#include <inspector/reader.hpp>

using namespace inspector;

namespace {
static constexpr auto kMaxAttempt = 32;
static constexpr auto kEventQueueName = "inspector-trace-test";
}  // namespace

class TracerTestFixture : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    details::Config::Get().write_max_attempt = kMaxAttempt;
    details::Config::Get().queue_system_unique_name = kEventQueueName;
    details::Config::Get().queue_remove_on_exit = true;
  }

  std::vector<std::string> Consume() {
    Reader reader;
    std::vector<std::string> events;
    for (auto&& event : reader) {
      events.push_back(std::move(event));
    }
    return events;
  }

  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(TracerTestFixture, TestSyncBegin) {
  SyncBegin("TestSync", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
            MakeKwarg("foo_b", 2));

  auto events = Consume();
  ASSERT_EQ(events.size(), 1);
  auto event = TraceEvent::Parse(events[0]);

  ASSERT_NE(event.Timestamp(), 0);
  ASSERT_EQ(event.ProcessId(), details::GetProcessId());
  ASSERT_EQ(event.ThreadId(), details::GetThreadId());
  ASSERT_EQ(event.Type(), kSyncBeginTag);
  ASSERT_EQ(event.Name(), "TestSync");
  ASSERT_EQ(event.Payload(), "testing|a|1|3.54|foo_a=45|foo_b=2");
}

TEST_F(TracerTestFixture, TestSyncEnd) {
  SyncEnd("TestSync");

  auto events = Consume();
  ASSERT_EQ(events.size(), 1);
  auto event = TraceEvent::Parse(events[0]);

  ASSERT_NE(event.Timestamp(), 0);
  ASSERT_EQ(event.ProcessId(), details::GetProcessId());
  ASSERT_EQ(event.ThreadId(), details::GetThreadId());
  ASSERT_EQ(event.Type(), kSyncEndTag);
  ASSERT_EQ(event.Name(), "TestSync");
  ASSERT_EQ(event.Payload(), "");
}

TEST_F(TracerTestFixture, TestAsyncBegin) {
  AsyncBegin("TestAsync", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
             MakeKwarg("foo_b", 2));

  auto events = Consume();
  ASSERT_EQ(events.size(), 1);
  auto event = TraceEvent::Parse(events[0]);

  ASSERT_NE(event.Timestamp(), 0);
  ASSERT_EQ(event.ProcessId(), details::GetProcessId());
  ASSERT_EQ(event.ThreadId(), details::GetThreadId());
  ASSERT_EQ(event.Type(), kAsyncBeginTag);
  ASSERT_EQ(event.Name(), "TestAsync");
  ASSERT_EQ(event.Payload(), "testing|a|1|3.54|foo_a=45|foo_b=2");
}

TEST_F(TracerTestFixture, TestAsyncInstance) {
  AsyncInstance("TestAsync", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
                MakeKwarg("foo_b", 2));

  auto events = Consume();
  ASSERT_EQ(events.size(), 1);
  auto event = TraceEvent::Parse(events[0]);

  ASSERT_NE(event.Timestamp(), 0);
  ASSERT_EQ(event.ProcessId(), details::GetProcessId());
  ASSERT_EQ(event.ThreadId(), details::GetThreadId());
  ASSERT_EQ(event.Type(), kAsyncInstanceTag);
  ASSERT_EQ(event.Name(), "TestAsync");
  ASSERT_EQ(event.Payload(), "testing|a|1|3.54|foo_a=45|foo_b=2");
}

TEST_F(TracerTestFixture, TestAsyncEnd) {
  AsyncEnd("TestAsync", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
           MakeKwarg("foo_b", 2));

  auto events = Consume();
  ASSERT_EQ(events.size(), 1);
  auto event = TraceEvent::Parse(events[0]);

  ASSERT_NE(event.Timestamp(), 0);
  ASSERT_EQ(event.ProcessId(), details::GetProcessId());
  ASSERT_EQ(event.ThreadId(), details::GetThreadId());
  ASSERT_EQ(event.Type(), kAsyncEndTag);
  ASSERT_EQ(event.Name(), "TestAsync");
  ASSERT_EQ(event.Payload(), "testing|a|1|3.54|foo_a=45|foo_b=2");
}

TEST_F(TracerTestFixture, TestFlowBegin) {
  FlowBegin("TestFlow", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
            MakeKwarg("foo_b", 2));

  auto events = Consume();
  ASSERT_EQ(events.size(), 1);
  auto event = TraceEvent::Parse(events[0]);

  ASSERT_NE(event.Timestamp(), 0);
  ASSERT_EQ(event.ProcessId(), details::GetProcessId());
  ASSERT_EQ(event.ThreadId(), details::GetThreadId());
  ASSERT_EQ(event.Type(), kFlowBeginTag);
  ASSERT_EQ(event.Name(), "TestFlow");
  ASSERT_EQ(event.Payload(), "testing|a|1|3.54|foo_a=45|foo_b=2");
}

TEST_F(TracerTestFixture, TestFlowInstance) {
  FlowInstance("TestFlow", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
               MakeKwarg("foo_b", 2));

  auto events = Consume();
  ASSERT_EQ(events.size(), 1);
  auto event = TraceEvent::Parse(events[0]);

  ASSERT_NE(event.Timestamp(), 0);
  ASSERT_EQ(event.ProcessId(), details::GetProcessId());
  ASSERT_EQ(event.ThreadId(), details::GetThreadId());
  ASSERT_EQ(event.Type(), kFlowInstanceTag);
  ASSERT_EQ(event.Name(), "TestFlow");
  ASSERT_EQ(event.Payload(), "testing|a|1|3.54|foo_a=45|foo_b=2");
}

TEST_F(TracerTestFixture, TestFlowEnd) {
  FlowEnd("TestFlow", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
          MakeKwarg("foo_b", 2));

  auto events = Consume();
  ASSERT_EQ(events.size(), 1);
  auto event = TraceEvent::Parse(events[0]);

  ASSERT_NE(event.Timestamp(), 0);
  ASSERT_EQ(event.ProcessId(), details::GetProcessId());
  ASSERT_EQ(event.ThreadId(), details::GetThreadId());
  ASSERT_EQ(event.Type(), kFLowEndTag);
  ASSERT_EQ(event.Name(), "TestFlow");
  ASSERT_EQ(event.Payload(), "testing|a|1|3.54|foo_a=45|foo_b=2");
}