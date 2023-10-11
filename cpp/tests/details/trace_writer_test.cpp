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

#include <vector>

#include <inspector/config.hpp>
#include <inspector/details/queue.hpp>
#include <inspector/details/trace_writer.hpp>
#include <inspector/trace_event.hpp>

using namespace inspector;

namespace {
static constexpr auto kEventQueueName = "inspector-trace_writer-test";
}  // namespace

class TraceWriterTestFixture : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { Config::setEventQueueName(kEventQueueName); }

  static TraceEvent Consume() {
    std::vector<uint8_t> event;
    details::eventQueue().consume(event);
    return TraceEvent(std::move(event));
  }

  void SetUp() override {}
  void TearDown() override {
    // BUG: This will result in seg fault when testing multiple test-cases
    Config::removeEventQueue();
  }
};

TEST_F(TraceWriterTestFixture, TestWriteTraceEvent) {
  std::string test_event = "testing";
  details::writeTraceEvent(1, 1, test_event.c_str());

  // Testing for written event
  auto event = Consume();
  ASSERT_EQ(event.type(), 1);
  ASSERT_EQ(event.category(), 1);
  ASSERT_TRUE(event.name());
  ASSERT_EQ(std::string{event.name()}, test_event);
  ASSERT_EQ(event.debugArgs().size(), 0);
}
