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

#include "tests/cpp/trace_event.hpp"

using namespace inspector;

class TracerTestFixture : public ::testing::Test {
 protected:
  static constexpr auto kMaxAttempt = 32;
  static Reader reader_;

  static void SetUpTestSuite() {
    Config config;
    config.max_attempt = kMaxAttempt;
    config.remove = true;
    Writer::SetConfig(config);
  }

  static void TearDownTestSuite() {}

  static std::vector<std::string> Consume() {
    std::vector<std::string> events;
    for (auto&& event : reader_) {
      events.push_back(std::move(event));
    }
    return events;
  }

  void SetUp() override {}
  void TearDown() override {}
};

Reader TracerTestFixture::reader_;

TEST_F(TracerTestFixture, TestSyncEventWithArgs) {
  SyncBegin("TestSync", "testing", 'a', 1, 3.54);

  auto events = Consume();
  ASSERT_EQ(events.size(), 1);
  auto event = inspector::testing::TraceEvent::Parse(events[0]);

  ASSERT_NE(event.timestamp, 0);
  ASSERT_EQ(event.pid, getpid());
  ASSERT_EQ(event.tid, gettid());
  ASSERT_EQ(event.type, 'B');
  ASSERT_EQ(event.name, "TestSync");
  ASSERT_EQ(event.payload, "testing|a|1|3.54");
}

TEST_F(TracerTestFixture, TestSyncEventWithKwargs) {
  SyncBegin("TestSync", MakeKwarg("a", "testing"), MakeKwarg("b", 'a'),
            MakeKwarg("c", 1), MakeKwarg("d", 3.54));

  auto events = Consume();
  ASSERT_EQ(events.size(), 1);
  auto event = inspector::testing::TraceEvent::Parse(events[0]);

  ASSERT_NE(event.timestamp, 0);
  ASSERT_EQ(event.pid, getpid());
  ASSERT_EQ(event.tid, gettid());
  ASSERT_EQ(event.type, 'B');
  ASSERT_EQ(event.name, "TestSync");
  ASSERT_EQ(event.payload, "a=testing|b=a|c=1|d=3.54");
}