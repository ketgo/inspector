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

#include <inspector/config.hpp>
#include <inspector/details/event_queue.hpp>
#include <inspector/details/trace_writer.hpp>

using namespace inspector;

namespace {
static constexpr auto kMaxAttempt = 32;
static constexpr auto kEventQueueName = "inspector-trace_writer-test";
}  // namespace

class TraceWriterTestFixture : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { Config::setEventQueueName(kEventQueueName); }

  static std::string Consume() {
    std::string message;
    details::eventQueue().consume(message);
    return message;
  }

  void SetUp() override {}
  void TearDown() override { Config::removeEventQueue(); }
};

TEST_F(TraceWriterTestFixture, TestWriteEvent) {
  std::string test_event = "testing";
  details::writeEvent(1, 1, test_event.c_str());

  // Testing for written event
  const auto event = Consume();
  ASSERT_EQ(test_event, event);
}
