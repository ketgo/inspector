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

#include <inspector/details/tracer.hpp>

#include "tests/cpp/trace_event.hpp"

using namespace inspector;

namespace {

constexpr auto kTestTraceEventType = 'T';
constexpr auto kTestTraceEventName = "TestTraceEvent";

}  // namespace

TEST(TracerTestFixture, TraceEventConstructor) {
  details::TraceEvent trace_event(kTestTraceEventType, kTestTraceEventName);

  auto event = inspector::testing::TraceEvent::Parse(trace_event.String());
  ASSERT_NE(event.timestamp, 0);
  ASSERT_NE(event.pid, 0);
  ASSERT_NE(event.tid, 0);
  ASSERT_EQ(event.type, kTestTraceEventType);
  ASSERT_EQ(event.name, kTestTraceEventName);
  ASSERT_EQ(event.payload, "");
}

TEST(TracerTestFixture, TraceEventWithSetArgs) {
  details::TraceEvent trace_event(kTestTraceEventType, kTestTraceEventName);
  trace_event.SetArgs("testing", 'a', 1, 3.54);

  auto event = inspector::testing::TraceEvent::Parse(trace_event.String());
  ASSERT_NE(event.timestamp, 0);
  ASSERT_NE(event.pid, 0);
  ASSERT_NE(event.tid, 0);
  ASSERT_EQ(event.type, kTestTraceEventType);
  ASSERT_EQ(event.name, kTestTraceEventName);
  ASSERT_EQ(event.payload, "testing|a|1|3.54");
}

TEST(TracerTestFixture, TraceEventWithSetKwargs) {
  details::TraceEvent trace_event(kTestTraceEventType, kTestTraceEventName);
  trace_event.SetKwargs(details::TraceEvent::MakeKwarg("a", "testing"),
                        details::TraceEvent::MakeKwarg("b", 'a'),
                        details::TraceEvent::MakeKwarg("c", 1),
                        details::TraceEvent::MakeKwarg("d", 3.54));

  auto event = inspector::testing::TraceEvent::Parse(trace_event.String());
  ASSERT_NE(event.timestamp, 0);
  ASSERT_NE(event.pid, 0);
  ASSERT_NE(event.tid, 0);
  ASSERT_EQ(event.type, kTestTraceEventType);
  ASSERT_EQ(event.name, kTestTraceEventName);
  ASSERT_EQ(event.payload, "a=testing|b=a|c=1|d=3.54");
}