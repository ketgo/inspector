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
#include <queue>

#include "tools/common/storage/block_builder.hpp"
#include "tools/common/storage/block_reader.hpp"
#include "tools/common/storage/testing.hpp"
#include "utils/random.hpp"

using namespace inspector::tools;

namespace {
constexpr auto kBlockSize = 1024;
}

class BlockTestFixture : public data::TestHarness, public ::testing::Test {};

TEST_F(BlockTestFixture, TestBlockBuilderCtor) {
  data::BlockBuilder builder(kBlockSize);

  ASSERT_EQ(builder.count(), 0);
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

TEST_F(BlockTestFixture, TestBlockBuilderAndReader) {
  utils::RandomNumberGenerator<data::timestamp_t> rand(100, 1000);

  data::BlockBuilder builder(kBlockSize);
  bool stop = false;
  std::size_t i = 0;
  std::priority_queue<data::timestamp_t> queue;
  do {
    data::timestamp_t timestamp = i * rand() + 1;
    queue.push(timestamp);
    std::string record = "test-data-" + std::to_string(i);
    stop = builder.add(timestamp, record.data(), record.size());
    ++i;
  } while (!stop);
  data::File file("block", tempDir());
  builder.flush(file);

  data::BlockReader reader(file);

  ASSERT_EQ(reader.count(), i);
  i = 0;
  for (const auto& entry : reader) {
    data::timestamp_t timestamp = queue.top();
    queue.pop();
    std::string record = "test-data-" + std::to_string(i);
    ASSERT_EQ(entry.first, timestamp);
    ASSERT_EQ(entry.second.second, record.size());
    ASSERT_EQ(
        std::memcmp(entry.second.first, record.data(), entry.second.second), 0);
  }
}