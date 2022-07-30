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

#include <inspector/logging.hpp>

using namespace inspector;

namespace {

class MockLogger : public Logger {
 public:
  MockLogger() {}
  void operator<<(const std::string& log) { message_ = log; }
  const std::string& Message() const { return message_; }

 private:
  std::string message_;
};

}  // namespace

TEST(LoggingTestFixture, Log) {
  MockLogger info_logger, warn_logger;

  RegisterLogger(LogLevel::INFO, info_logger);
  RegisterLogger(LogLevel::WARN, warn_logger);

  LOG_INFO << "testing_" << 0;
  ASSERT_EQ(info_logger.Message(), "testing_0");
  ASSERT_EQ(warn_logger.Message(), "");

  LOG_WARN << "testing_" << 1;
  ASSERT_EQ(info_logger.Message(), "testing_0");
  ASSERT_EQ(warn_logger.Message(), "testing_1");
}