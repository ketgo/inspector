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

#include <inspector/details/trace_writer.hpp>

using namespace inspector;

namespace {
static constexpr auto kMaxAttempt = 32;
static constexpr auto kEventQueueName = "inspector-trace_writer-test";
}  // namespace

class TraceWriterTestFixture : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    details::Config::Get().max_write_attempt = kMaxAttempt;
    details::Config::Get().queue_system_unique_name = kEventQueueName;
    details::Config::Get().queue_remove_on_exit = true;
  }

  static std::string Consume() {
    static auto queue =
        details::system::GetSharedObject<details::EventQueue>(kEventQueueName);
    details::EventQueue::ReadSpan span;
    queue->Consume(span);
    return std::string(span.Data(), span.Size());
  }

  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(TraceWriterTestFixture, TestWrite) {
  std::string test_event = "testing";
  details::TraceWriter::Get().Write(test_event);

  // Testing for written event
  ASSERT_EQ(test_event, Consume());
}
