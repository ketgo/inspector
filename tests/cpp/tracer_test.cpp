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

#include <memory>

#include <inspector/tracer.hpp>
#include <inspector/reader.hpp>

#include "utils/strings.hpp"

using namespace inspector;

class TracerTestFixture : public ::testing::Test {
 protected:
  static constexpr auto kMaxAttempt = 32;
  std::shared_ptr<Reader> reader_;

  void SetUp() override {
    Config config;
    config.max_attempt = kMaxAttempt;
    config.remove = true;
    Reader::SetConfig(config);
    reader_ = std::make_shared<Reader>();
  }

  std::vector<std::string> Consume() {
    std::vector<std::string> events;
    for (auto&& event : *reader_) {
      events.push_back(std::move(event));
    }
    return events;
  }
};

TEST_F(TracerTestFixture, TestSyncEvent) {
  SyncBegin("TestSync", 'a', 1, 3.54);

  auto events = Consume();
  ASSERT_EQ(events.size(), 1);
  int64_t timestamp;
  int32_t pid, tid;
  char type, delimeter;
  std::string event;
  std::istringstream stream(events[0]);
  stream >> timestamp >> delimeter >> pid >> delimeter >> tid >> delimeter >>
      type >> delimeter >> event;
  ASSERT_EQ(delimeter, '|');
  ASSERT_NE(timestamp, 0);
  ASSERT_NE(pid, 0);
  ASSERT_NE(tid, 0);
  ASSERT_EQ(type, 'B');
  ASSERT_EQ(event, "TestSync|a|1|3.54");
}