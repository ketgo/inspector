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

#include <inspector/details/trace_event.hpp>

#include "utils/strings.hpp"

using namespace inspector;

namespace {

constexpr auto kTestTraceEventPhase = 'T';
constexpr auto kTestTraceEventName = "TestTraceEvent";

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

TEST(TraceEventTestFixture, TestConstructor) {
  details::TraceEvent trace_event(kTestTraceEventPhase, kTestTraceEventName);

  auto event = Event::Parse(trace_event.String());
  ASSERT_EQ(event.header.type, 0);
  ASSERT_NE(event.header.timestamp, 0);
  ASSERT_EQ(event.header.pid, details::system::GetProcessId());
  ASSERT_EQ(event.header.tid, details::system::GetThreadId());
  ASSERT_EQ(event.phase, kTestTraceEventPhase);
  ASSERT_EQ(event.name, kTestTraceEventName);
  ASSERT_EQ(event.args, "");
}

TEST(TraceEventTestFixture, TestSetArgs) {
  details::TraceEvent trace_event(kTestTraceEventPhase, kTestTraceEventName);
  trace_event.SetArgs("testing", 'a', 1, 3.54);

  auto event = Event::Parse(trace_event.String());
  ASSERT_EQ(event.header.type, 0);
  ASSERT_NE(event.header.timestamp, 0);
  ASSERT_EQ(event.header.pid, details::system::GetProcessId());
  ASSERT_EQ(event.header.tid, details::system::GetThreadId());
  ASSERT_EQ(event.phase, kTestTraceEventPhase);
  ASSERT_EQ(event.name, kTestTraceEventName);
  ASSERT_EQ(event.args, "testing|a|1|3.54");
}
