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

using namespace inspector;

namespace {
static constexpr auto kMaxAttempt = 32;
}

class TraceWriterTestFixture : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    details::Config::Get().write_max_attempt = kMaxAttempt;
    details::Config::Get().queue_system_unique_name =
        "inspector-trace_writer-test";
    details::Config::Get().queue_remove_on_exit = true;
  }

  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(TraceWriterTestFixture, TestWrite) {
  std::string test_event = "testing";
  details::TraceWriter::Get().Write(test_event);

  // Testing for written event
  auto queue = details::shared_object::GetOrCreate<details::EventQueue>(
      details::Config::Get().queue_system_unique_name);
  details::EventQueue::ReadSpan span;
  ASSERT_EQ(queue->Consume(span), details::EventQueue::Status::OK);
  ASSERT_EQ(test_event, std::string(span.Data(), span.Size()));
}
