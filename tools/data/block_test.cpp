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

#include <cstring>

#include "tools/data/block_builder.hpp"
#include "tools/data/block_reader.hpp"
#include "tools/data/testing.hpp"

using namespace inspector::tools;

namespace {
constexpr auto kBlockSize = 1024;
}

class BlockTestFixture : public data::TestHarness, public ::testing::Test {};

TEST_F(BlockTestFixture, TestBlockBuilderCtor) {
  data::BlockBuilder builder(kBlockSize);

  ASSERT_EQ(builder.count(), 0);
}

TEST_F(BlockTestFixture, TestBlockBuilderAdd) {
  data::BlockBuilder builder(kBlockSize);
  const std::string kData1 = "data_1";
  const std::string kData2 = "data_2";

  ASSERT_TRUE(builder.add(10, kData1.data(), kData1.size()));
  ASSERT_TRUE(builder.add(100, kData2.data(), kData2.size()));

  ASSERT_EQ(builder.count(), 2);
  ASSERT_EQ(builder.startTimestamp(), 10);
  ASSERT_EQ(builder.endTimestamp(), 100);
}

TEST_F(BlockTestFixture, TestBlockBuilderFlush) {
  data::BlockBuilder builder(kBlockSize);
  const std::string kData1 = "data_1";
  const std::string kData2 = "data_2";

  ASSERT_TRUE(builder.add(10, kData1.data(), kData1.size()));
  ASSERT_TRUE(builder.add(100, kData2.data(), kData2.size()));

  data::File file("block", tempDir());
  builder.flush(file);
  file.sync();

  ASSERT_TRUE(tempDir().fileExists("block"));
  ASSERT_EQ(tempDir().readFile("block").size(), kBlockSize);
}

TEST_F(BlockTestFixture, TestBlockReader) {
  data::BlockBuilder builder(kBlockSize);
  const std::string kData1 = "data_1";
  const std::string kData2 = "data_2";

  ASSERT_TRUE(builder.add(10, kData1.data(), kData1.size()));
  ASSERT_TRUE(builder.add(100, kData2.data(), kData2.size()));

  data::File file("block", tempDir());
  builder.flush(file);
  file.sync();

  data::BlockReader reader(file);

  ASSERT_EQ(reader.count(), 2);
  ASSERT_EQ(reader.startTimestamp(), 10);
  ASSERT_EQ(reader.endTimestamp(), 100);
  auto it = reader.begin();
  ASSERT_NE(it, reader.end());
  ASSERT_EQ(it->first, 10);
  ASSERT_EQ(it->second.second, kData1.size());
  ASSERT_EQ(std::memcmp(it->second.first, kData1.data(), kData1.size()), 0);
  ++it;
  ASSERT_NE(it, reader.end());
  ASSERT_EQ(it->first, 100);
  ASSERT_EQ(it->second.second, kData2.size());
  ASSERT_EQ(std::memcmp(it->second.first, kData2.data(), kData2.size()), 0);
  ++it;
  ASSERT_EQ(it, reader.end());
}