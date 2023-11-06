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

#include "tools/reader/event.hpp"

using namespace inspector;

TEST(EventTestFixture, TestParser) {
  details::TraceEvent trace_event('T', "TestTraceEvent");
  std::string event_str = trace_event.String();

  auto event = Event::Parse(event_str.data(), event_str.size());
  ASSERT_EQ(event.Type(), 0);
  ASSERT_NE(event.Timestamp(), 0);
  ASSERT_EQ(event.Pid(), details::system::GetProcessId());
  ASSERT_EQ(event.Tid(), details::system::GetThreadId());
  ASSERT_EQ(event.Payload(), "T|TestTraceEvent");
}