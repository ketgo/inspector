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
#include <inspector/trace_reader.hpp>

#include "cpp/tests/testing.hpp"

using namespace inspector;

namespace {
static constexpr auto kEventQueueName = "inspector-trace-rw-test";
}  // namespace

class TraceReaderWriterTestFixture : public ::testing::Test {
 protected:
  static void SetUpTestSuite() { Config::setEventQueueName(kEventQueueName); }
  static void TearDownTestSuite() { inspector::testing::removeEventQueue(); }
  void SetUp() override {}
  void TearDown() override { inspector::testing::emptyEventQueue(); }
};

TEST_F(TraceReaderWriterTestFixture, TestWriteAndReadTraceEvent) {
  std::string test_event = "testing";
  details::writeTraceEvent(1, test_event.c_str());
  auto event = readTraceEvent();
  ASSERT_EQ(event.type(), 1);
  ASSERT_TRUE(event.name());
  ASSERT_EQ(std::string{event.name()}, test_event);
  ASSERT_EQ(event.debugArgs().size(), 0);
}
