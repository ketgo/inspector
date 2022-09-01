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

#include "utils/strings.hpp"

using namespace inspector;

namespace {

static constexpr auto kMaxAttempt = 32;
static constexpr auto kEventQueueName = "inspector-trace-test";

struct Event {
  details::EventHeader header;
  char phase;
  std::string name;
  std::string args;

  static Event Parse(const std::string& data) {
    Event event;

    const auto* header_ =
        reinterpret_cast<const details::EventHeader*>(data.data());
    event.header.type = header_->type;
    event.header.timestamp = header_->timestamp;
    event.header.pid = header_->pid;
    event.header.tid = header_->tid;

    auto split =
        utils::SplitOnce(data.substr(sizeof(details::EventHeader)), "|");
    event.phase = split.first[0];
    split = utils::SplitOnce(split.second, "|");
    event.name = split.first;
    event.args = split.second;

    return event;
  }
};

}  // namespace

class TracerTestFixture : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    details::Config::Get().write_max_attempt = kMaxAttempt;
    details::Config::Get().queue_system_unique_name = kEventQueueName;
    details::Config::Get().queue_remove_on_exit = true;
  }

  static std::string Consume() {
    static auto queue =
        details::system::GetSharedObject<details::EventQueue>(kEventQueueName);
    details::EventQueue::ReadSpan span;
    queue->Consume(span);
    return std::string(span.Data(), span.Size());
  }

  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(TracerTestFixture, TestSyncBegin) {
  SyncBegin("TestSync", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
            MakeKwarg("foo_b", 2));

  auto event = Event::Parse(Consume());
  ASSERT_EQ(event.header.type, 0);
  ASSERT_NE(event.header.timestamp, 0);
  ASSERT_EQ(event.header.pid, details::system::GetProcessId());
  ASSERT_EQ(event.header.tid, details::system::GetThreadId());
  ASSERT_EQ(event.phase, kSyncBeginTag);
  ASSERT_EQ(event.name, "TestSync");
  ASSERT_EQ(event.args, "testing|a|1|3.54|foo_a=45|foo_b=2");
}

TEST_F(TracerTestFixture, TestSyncEnd) {
  SyncEnd("TestSync");

  auto event = Event::Parse(Consume());
  ASSERT_EQ(event.header.type, 0);
  ASSERT_NE(event.header.timestamp, 0);
  ASSERT_EQ(event.header.pid, details::system::GetProcessId());
  ASSERT_EQ(event.header.tid, details::system::GetThreadId());
  ASSERT_EQ(event.phase, kSyncEndTag);
  ASSERT_EQ(event.name, "TestSync");
  ASSERT_EQ(event.args, "");
}

TEST_F(TracerTestFixture, TestAsyncBegin) {
  AsyncBegin("TestAsync", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
             MakeKwarg("foo_b", 2));

  auto event = Event::Parse(Consume());
  ASSERT_EQ(event.header.type, 0);
  ASSERT_NE(event.header.timestamp, 0);
  ASSERT_EQ(event.header.pid, details::system::GetProcessId());
  ASSERT_EQ(event.header.tid, details::system::GetThreadId());
  ASSERT_EQ(event.phase, kAsyncBeginTag);
  ASSERT_EQ(event.name, "TestAsync");
  ASSERT_EQ(event.args, "testing|a|1|3.54|foo_a=45|foo_b=2");
}

TEST_F(TracerTestFixture, TestAsyncInstance) {
  AsyncInstance("TestAsync", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
                MakeKwarg("foo_b", 2));

  auto event = Event::Parse(Consume());
  ASSERT_EQ(event.header.type, 0);
  ASSERT_NE(event.header.timestamp, 0);
  ASSERT_EQ(event.header.pid, details::system::GetProcessId());
  ASSERT_EQ(event.header.tid, details::system::GetThreadId());
  ASSERT_EQ(event.phase, kAsyncInstanceTag);
  ASSERT_EQ(event.name, "TestAsync");
  ASSERT_EQ(event.args, "testing|a|1|3.54|foo_a=45|foo_b=2");
}

TEST_F(TracerTestFixture, TestAsyncEnd) {
  AsyncEnd("TestAsync", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
           MakeKwarg("foo_b", 2));

  auto event = Event::Parse(Consume());
  ASSERT_EQ(event.header.type, 0);
  ASSERT_NE(event.header.timestamp, 0);
  ASSERT_EQ(event.header.pid, details::system::GetProcessId());
  ASSERT_EQ(event.header.tid, details::system::GetThreadId());
  ASSERT_EQ(event.phase, kAsyncEndTag);
  ASSERT_EQ(event.name, "TestAsync");
  ASSERT_EQ(event.args, "testing|a|1|3.54|foo_a=45|foo_b=2");
}

TEST_F(TracerTestFixture, TestFlowBegin) {
  FlowBegin("TestFlow", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
            MakeKwarg("foo_b", 2));

  auto event = Event::Parse(Consume());
  ASSERT_EQ(event.header.type, 0);
  ASSERT_NE(event.header.timestamp, 0);
  ASSERT_EQ(event.header.pid, details::system::GetProcessId());
  ASSERT_EQ(event.header.tid, details::system::GetThreadId());
  ASSERT_EQ(event.phase, kFlowBeginTag);
  ASSERT_EQ(event.name, "TestFlow");
  ASSERT_EQ(event.args, "testing|a|1|3.54|foo_a=45|foo_b=2");
}

TEST_F(TracerTestFixture, TestFlowInstance) {
  FlowInstance("TestFlow", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
               MakeKwarg("foo_b", 2));

  auto event = Event::Parse(Consume());
  ASSERT_EQ(event.header.type, 0);
  ASSERT_NE(event.header.timestamp, 0);
  ASSERT_EQ(event.header.pid, details::system::GetProcessId());
  ASSERT_EQ(event.header.tid, details::system::GetThreadId());
  ASSERT_EQ(event.phase, kFlowInstanceTag);
  ASSERT_EQ(event.name, "TestFlow");
  ASSERT_EQ(event.args, "testing|a|1|3.54|foo_a=45|foo_b=2");
}

TEST_F(TracerTestFixture, TestFlowEnd) {
  FlowEnd("TestFlow", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
          MakeKwarg("foo_b", 2));

  auto event = Event::Parse(Consume());
  ASSERT_EQ(event.header.type, 0);
  ASSERT_NE(event.header.timestamp, 0);
  ASSERT_EQ(event.header.pid, details::system::GetProcessId());
  ASSERT_EQ(event.header.tid, details::system::GetThreadId());
  ASSERT_EQ(event.phase, kFlowEndTag);
  ASSERT_EQ(event.name, "TestFlow");
  ASSERT_EQ(event.args, "testing|a|1|3.54|foo_a=45|foo_b=2");
}