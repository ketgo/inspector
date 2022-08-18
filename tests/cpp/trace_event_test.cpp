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

#include <inspector/trace_event.hpp>

using namespace inspector;

namespace {

constexpr auto kTestTraceEventType = 'T';
constexpr auto kTestTraceEventName = "TestTraceEvent";

}  // namespace

TEST(TraceEventTestFixture, TestConstructor) {
  TraceEvent trace_event(kTestTraceEventType, kTestTraceEventName);

  auto event = TraceEvent::Parse(trace_event.String());
  ASSERT_NE(event.Timestamp(), 0);
  ASSERT_EQ(event.ProcessId(), details::GetProcessId());
  ASSERT_EQ(event.ThreadId(), details::GetThreadId());
  ASSERT_EQ(event.Type(), kTestTraceEventType);
  ASSERT_EQ(event.Name(), kTestTraceEventName);
  ASSERT_EQ(event.Payload(), "");
}

TEST(TraceEventTestFixture, TestSetArgs) {
  TraceEvent trace_event(kTestTraceEventType, kTestTraceEventName);
  trace_event.SetArgs("testing", 'a', 1, 3.54);

  auto event = TraceEvent::Parse(trace_event.String());
  ASSERT_NE(event.Timestamp(), 0);
  ASSERT_EQ(event.ProcessId(), details::GetProcessId());
  ASSERT_EQ(event.ThreadId(), details::GetThreadId());
  ASSERT_EQ(event.Type(), kTestTraceEventType);
  ASSERT_EQ(event.Name(), kTestTraceEventName);
  ASSERT_EQ(event.Payload(), "testing|a|1|3.54");
}
