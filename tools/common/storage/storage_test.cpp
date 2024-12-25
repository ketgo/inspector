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

#include "tools/common/storage/storage.hpp"

#include <gtest/gtest.h>

#include <cstring>
#include <queue>
#include <unordered_map>
#include <vector>

#include "tools/common/storage/block.hpp"
#include "tools/common/storage/testing.hpp"
#include "utils/random.hpp"

using namespace inspector::tools::storage;

namespace {
constexpr auto kBlockSize = 1024;
}

class StorageTestFixture : public TestHarness, public ::testing::Test {
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

TEST_F(StorageTestFixture, TestWriteAndRead) {
  constexpr auto kRecordCount = 1000;
  using TimestampQueue =
      std::priority_queue<timestamp_t, std::vector<timestamp_t>,
                          std::greater<timestamp_t>>;
  using TimestampRecordsMap =
      std::unordered_map<timestamp_t, std::vector<std::string>>;

  TimestampQueue queue;
  TimestampRecordsMap records_map;
  utils::RandomNumberGenerator<timestamp_t> rand(100, 1000);

  Writer writer(tempDir().path(), kBlockSize);

  std::size_t total_size = 0;
  for (auto i = 0; i < kRecordCount; ++i) {
    timestamp_t timestamp = rand();
    std::string record = "test-data-" + std::to_string(i);
    writer.write({timestamp, record.data(), record.size()});
    queue.push(timestamp);
    records_map[timestamp].emplace_back(record);
    total_size += record.size();
  }
  writer.flush();

  ASSERT_GE(tempDir().listFiles().size(), total_size / kBlockSize);

  Reader reader{tempDir().path()};
  std::size_t i = 0;
  for (const auto& entry : reader) {
    ASSERT_EQ(entry.timestamp, queue.top());
    queue.pop();
    ASSERT_TRUE(isOneOf(entry, records_map[entry.timestamp]));
    ++i;
  }
  ASSERT_EQ(i, kRecordCount);
}