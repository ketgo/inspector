// Copyright 2022 Ketan Goyal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include <memory>

#include <inspector/writer.hpp>

using namespace inspector;

class WriterTestFixture : public ::testing::Test {
 protected:
  static constexpr auto kMaxAttempt = 32;
  static Config config_;
  std::shared_ptr<Writer> writer_;

  static void SetUpTestSuite() {
    config_.max_attempt = kMaxAttempt;
    config_.queue_system_unique_name = "inspector-writer-test";
    config_.remove = true;
    Writer::SetConfig(config_);
  }

  void SetUp() override { writer_ = std::make_shared<Writer>(); }
  void TearDown() override {}
};

Config WriterTestFixture::config_;

TEST_F(WriterTestFixture, TestWrite) {
  std::string test_event = "testing";
  writer_->Write(test_event);

  // Testing for written event
  auto queue = details::shared_object::GetOrCreate<details::EventQueue>(
      config_.queue_system_unique_name);
  details::EventQueue::ReadSpan span;
  ASSERT_EQ(queue->Consume(span), details::EventQueue::Status::OK);
  ASSERT_EQ(test_event, std::string(span.Data(), span.Size()));
}
