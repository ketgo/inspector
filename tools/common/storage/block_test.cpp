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

#include "tools/common/storage/block.hpp"

#include <gtest/gtest.h>

#include <cstring>
#include <queue>
#include <unordered_map>
#include <vector>

#include "tools/common/storage/testing.hpp"
#include "utils/random.hpp"

using namespace inspector::tools::storage;

namespace {
constexpr auto kBlockSize = 1024;
}

class BlockTestFixture : public TestHarness, public ::testing::Test {
 protected:
  static bool isOneOf(const Record& record,
                      std::vector<std::string> candidates) {
    for (const auto& candidate : candidates) {
      if (record.size == candidate.size() &&
          std::memcmp(record.src, candidate.data(), record.size) == 0) {
        return true;
      }
    }
    return false;
  }
};

TEST_F(BlockTestFixture, TestBlockBuilderCtor) {
  BlockBuilder builder(kBlockSize);

  ASSERT_EQ(builder.count(), 0);
}

TEST_F(BlockTestFixture, TestBlockBuilderFlush) {
  BlockBuilder builder(kBlockSize);
  const std::string kData1 = "data_1";
  const std::string kData2 = "data_2";

  ASSERT_TRUE(builder.add({10, kData1.data(), kData1.size()}));
  ASSERT_TRUE(builder.add({100, kData2.data(), kData2.size()}));

  builder.flush(File{"block", tempDir().path()});

  ASSERT_TRUE(tempDir().fileExists("block"));
  ASSERT_EQ(tempDir().readFile("block").size(), kBlockSize);
}

TEST_F(BlockTestFixture, TestBlockBuilderAndReader) {
  using TimestampQueue =
      std::priority_queue<timestamp_t, std::vector<timestamp_t>,
                          std::greater<timestamp_t>>;
  using TimestampRecordsMap =
      std::unordered_map<timestamp_t, std::vector<std::string>>;

  TimestampQueue queue;
  TimestampRecordsMap records_map;
  utils::RandomNumberGenerator<timestamp_t> rand(100, 1000);

  BlockBuilder builder(kBlockSize);
  bool success = false;
  std::size_t i = 0;
  do {
    timestamp_t timestamp = rand();
    std::string record = "test-data-" + std::to_string(i);
    success = builder.add({timestamp, record.data(), record.size()});
    if (success) {
      queue.push(timestamp);
      records_map[timestamp].emplace_back(record);
      ++i;
    }
  } while (success);
  builder.flush(File{"block", tempDir().path()});

  BlockReader reader(File{"block", tempDir().path()});
  ASSERT_EQ(reader.count(), i);
  for (const auto& entry : reader) {
    ASSERT_EQ(entry.timestamp, queue.top());
    queue.pop();
    ASSERT_TRUE(isOneOf(entry, records_map[entry.timestamp]));
  }
}