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
#include <inspector/trace_reader.hpp>

#include "cpp/tests/testing.hpp"

using namespace inspector;

namespace {
static constexpr auto kEventQueueName = "inspector-trace-test";
}  // namespace

class TracerTestFixture : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { Config::setEventQueueName(kEventQueueName); }
  static void TearDownTestSuite() { inspector::testing::removeEventQueue(); }
  void SetUp() override {}
  void TearDown() override { inspector::testing::emptyEventQueue(); }
};

TEST_F(TracerTestFixture, TestSyncBegin) {
  syncBegin("TestSync", "testing", 'a', 1, 3.54, KWARG("test", 50));

  auto event = readTraceEvent();
  ASSERT_EQ(event.type(), static_cast<event_type_t>(EventType::kSyncBeginTag));
  ASSERT_NE(event.counter(), 0);
  ASSERT_NE(event.timestampNs(), 0);
  ASSERT_EQ(event.pid(), details::getPID());
  ASSERT_EQ(event.tid(), details::getTID());
  ASSERT_EQ(std::string{event.name()}, "TestSync");
  ASSERT_EQ(event.debugArgs().size(), 5);
  auto it = event.debugArgs().begin();
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_STRING);
  ASSERT_EQ(it->value<std::string>(), "testing");
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_CHAR);
  ASSERT_EQ(it->value<char>(), 'a');
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_INT32);
  ASSERT_EQ(it->value<int>(), 1);
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_DOUBLE);
  ASSERT_EQ(it->value<double>(), 3.54);
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_KWARG);
  const auto kwarg = it->value<KeywordArg>();
  ASSERT_EQ(std::strcmp("test", kwarg.name()), 0);
  ASSERT_EQ(kwarg.type(), DebugArg::Type::TYPE_INT32);
  ASSERT_EQ(kwarg.value<int32_t>(), 50);
  ++it;
  ASSERT_EQ(it, event.debugArgs().end());
}

TEST_F(TracerTestFixture, TestSyncEnd) {
  syncEnd("TestSync");

  auto event = readTraceEvent();
  ASSERT_EQ(event.type(), static_cast<event_type_t>(EventType::kSyncEndTag));
  ASSERT_NE(event.counter(), 0);
  ASSERT_NE(event.timestampNs(), 0);
  ASSERT_EQ(event.pid(), details::getPID());
  ASSERT_EQ(event.tid(), details::getTID());
  ASSERT_EQ(std::string{event.name()}, "TestSync");
  ASSERT_EQ(event.debugArgs().size(), 0);
}

TEST_F(TracerTestFixture, TestAsyncBegin) {
  asyncBegin("TestAsync", "testing", 'a', 1, 3.54);

  auto event = readTraceEvent();
  ASSERT_EQ(event.type(), static_cast<event_type_t>(EventType::kAsyncBeginTag));
  ASSERT_NE(event.counter(), 0);
  ASSERT_NE(event.timestampNs(), 0);
  ASSERT_EQ(event.pid(), details::getPID());
  ASSERT_EQ(event.tid(), details::getTID());
  ASSERT_EQ(std::string{event.name()}, "TestAsync");
  ASSERT_EQ(event.debugArgs().size(), 4);
  auto it = event.debugArgs().begin();
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_STRING);
  ASSERT_EQ(it->value<std::string>(), "testing");
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_CHAR);
  ASSERT_EQ(it->value<char>(), 'a');
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_INT32);
  ASSERT_EQ(it->value<int>(), 1);
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_DOUBLE);
  ASSERT_EQ(it->value<double>(), 3.54);
  ++it;
  ASSERT_EQ(it, event.debugArgs().end());
}

TEST_F(TracerTestFixture, TestAsyncInstance) {
  asyncInstance("TestAsync", "testing", 'a', 1, 3.54);

  auto event = readTraceEvent();
  ASSERT_EQ(event.type(),
            static_cast<event_type_t>(EventType::kAsyncInstanceTag));
  ASSERT_NE(event.counter(), 0);
  ASSERT_NE(event.timestampNs(), 0);
  ASSERT_EQ(event.pid(), details::getPID());
  ASSERT_EQ(event.tid(), details::getTID());
  ASSERT_EQ(std::string{event.name()}, "TestAsync");
  ASSERT_EQ(event.debugArgs().size(), 4);
  auto it = event.debugArgs().begin();
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_STRING);
  ASSERT_EQ(it->value<std::string>(), "testing");
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_CHAR);
  ASSERT_EQ(it->value<char>(), 'a');
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_INT32);
  ASSERT_EQ(it->value<int>(), 1);
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_DOUBLE);
  ASSERT_EQ(it->value<double>(), 3.54);
  ++it;
  ASSERT_EQ(it, event.debugArgs().end());
}

TEST_F(TracerTestFixture, TestAsyncEnd) {
  asyncEnd("TestAsync", "testing", 'a', 1, 3.54);

  auto event = readTraceEvent();
  ASSERT_EQ(event.type(), static_cast<event_type_t>(EventType::kAsyncEndTag));
  ASSERT_NE(event.counter(), 0);
  ASSERT_NE(event.timestampNs(), 0);
  ASSERT_EQ(event.pid(), details::getPID());
  ASSERT_EQ(event.tid(), details::getTID());
  ASSERT_EQ(std::string{event.name()}, "TestAsync");
  ASSERT_EQ(event.debugArgs().size(), 4);
  auto it = event.debugArgs().begin();
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_STRING);
  ASSERT_EQ(it->value<std::string>(), "testing");
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_CHAR);
  ASSERT_EQ(it->value<char>(), 'a');
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_INT32);
  ASSERT_EQ(it->value<int>(), 1);
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_DOUBLE);
  ASSERT_EQ(it->value<double>(), 3.54);
  ++it;
  ASSERT_EQ(it, event.debugArgs().end());
}

TEST_F(TracerTestFixture, TestFlowBegin) {
  flowBegin("TestFlow", "testing", 'a', 1, 3.54);

  auto event = readTraceEvent();
  ASSERT_EQ(event.type(), static_cast<event_type_t>(EventType::kFlowBeginTag));
  ASSERT_NE(event.counter(), 0);
  ASSERT_NE(event.timestampNs(), 0);
  ASSERT_EQ(event.pid(), details::getPID());
  ASSERT_EQ(event.tid(), details::getTID());
  ASSERT_EQ(std::string{event.name()}, "TestFlow");
  ASSERT_EQ(event.debugArgs().size(), 4);
  auto it = event.debugArgs().begin();
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_STRING);
  ASSERT_EQ(it->value<std::string>(), "testing");
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_CHAR);
  ASSERT_EQ(it->value<char>(), 'a');
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_INT32);
  ASSERT_EQ(it->value<int>(), 1);
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_DOUBLE);
  ASSERT_EQ(it->value<double>(), 3.54);
  ++it;
  ASSERT_EQ(it, event.debugArgs().end());
}

TEST_F(TracerTestFixture, TestFlowInstance) {
  flowInstance("TestFlow", "testing", 'a', 1, 3.54);

  auto event = readTraceEvent();
  ASSERT_EQ(event.type(),
            static_cast<event_type_t>(EventType::kFlowInstanceTag));
  ASSERT_NE(event.counter(), 0);
  ASSERT_NE(event.timestampNs(), 0);
  ASSERT_EQ(event.pid(), details::getPID());
  ASSERT_EQ(event.tid(), details::getTID());
  ASSERT_EQ(std::string{event.name()}, "TestFlow");
  ASSERT_EQ(event.debugArgs().size(), 4);
  auto it = event.debugArgs().begin();
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_STRING);
  ASSERT_EQ(it->value<std::string>(), "testing");
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_CHAR);
  ASSERT_EQ(it->value<char>(), 'a');
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_INT32);
  ASSERT_EQ(it->value<int>(), 1);
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_DOUBLE);
  ASSERT_EQ(it->value<double>(), 3.54);
  ++it;
  ASSERT_EQ(it, event.debugArgs().end());
}

TEST_F(TracerTestFixture, TestFlowEnd) {
  flowEnd("TestFlow", "testing", 'a', 1, 3.54);

  auto event = readTraceEvent();
  ASSERT_EQ(event.type(), static_cast<event_type_t>(EventType::kFlowEndTag));
  ASSERT_NE(event.counter(), 0);
  ASSERT_NE(event.timestampNs(), 0);
  ASSERT_EQ(event.pid(), details::getPID());
  ASSERT_EQ(event.tid(), details::getTID());
  ASSERT_EQ(std::string{event.name()}, "TestFlow");
  ASSERT_EQ(event.debugArgs().size(), 4);
  auto it = event.debugArgs().begin();
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_STRING);
  ASSERT_EQ(it->value<std::string>(), "testing");
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_CHAR);
  ASSERT_EQ(it->value<char>(), 'a');
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_INT32);
  ASSERT_EQ(it->value<int>(), 1);
  ++it;
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_DOUBLE);
  ASSERT_EQ(it->value<double>(), 3.54);
  ++it;
  ASSERT_EQ(it, event.debugArgs().end());
}
