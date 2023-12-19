/**
 * Copyright 2023 Ketan Goyal
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

using namespace inspector;

TEST(ConfigTestFixture, TestConstantEventQueueName) {
  ASSERT_EQ(Config::eventQueueName(), Config::eventQueueName());
}

TEST(ConfigTestFixture, TestTraceEnableDisable) {
  ASSERT_FALSE(Config::isTraceDisabled());  // By default tracing is enabled
  Config::disableTrace();
  ASSERT_TRUE(Config::isTraceDisabled());
  Config::enableTrace();
  ASSERT_FALSE(Config::isTraceDisabled());
}
