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

#include <inspector/tracer.hpp>
#include <inspector/reader.hpp>

using namespace inspector;

class TracerTestFixture : public ::testing::Test {
 protected:
  static constexpr auto kMaxAttempt = 32;
  Config config_;

  std::vector<std::string> Consume() {
    Reader reader;
    std::vector<std::string> events;
    for (auto&& event : reader) {
      events.push_back(std::move(event));
    }
    return events;
  }

  void SetUp() override {
    config_.max_attempt = kMaxAttempt;
    config_.queue_system_unique_name = "inspector-tracer-test";
    Reader::SetConfig(config_);
    config_.remove = true;
    Writer::SetConfig(config_);
  }

  void TearDown() override {}
};

TEST_F(TracerTestFixture, TestSyncEventWithArgs) {
  SyncBegin("TestSync", "testing", 'a', 1, 3.54, MakeKwarg("foo_a", "45"),
            MakeKwarg("foo_b", 2));

  auto events = Consume();
  ASSERT_EQ(events.size(), 1);
  auto event = TraceEvent::Parse(events[0]);

  ASSERT_NE(event.Timestamp(), 0);
  ASSERT_EQ(event.ProcessId(), details::GetProcessId());
  ASSERT_EQ(event.ThreadId(), details::GetThreadId());
  ASSERT_EQ(event.Type(), 'B');
  ASSERT_EQ(event.Name(), "TestSync");
  ASSERT_EQ(event.Payload(), "testing|a|1|3.54|foo_a=45|foo_b=2");
}
