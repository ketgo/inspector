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

#include "tools/common/storage/reader.hpp"
#include "tools/common/storage/testing.hpp"
#include "tools/common/storage/writer.hpp"

using namespace inspector::tools;

namespace {
constexpr auto kBlockSize = 1024;
}

class StorageTestFixture : public data::TestHarness, public ::testing::Test {};

TEST_F(StorageTestFixture, TestWriteAndRead) {
  constexpr auto kRecordCount = 1000;

  data::Writer writer(tempDir().path(), kBlockSize);

  std::size_t total_size = 0;
  for (auto i = 0; i < kRecordCount; ++i) {
    data::timestamp_t timestamp = i * 100 + 1;
    std::string record = "test-data-" + std::to_string(i);
    writer.write(timestamp, record.data(), record.size());
    total_size += record.size();
  }
  writer.flush();

  ASSERT_GE(tempDir().listFiles().size(), total_size / kBlockSize);

  data::Reader reader{tempDir().path()};
  std::size_t count = 0;
  auto it = reader.begin();
  while (count < 30) {
    ASSERT_NE(it, reader.end());
    std::cout << "[" << it->first << "] - "
              << std::string{reinterpret_cast<const char*>(it->second.first),
                             it->second.second}
              << "\n";
    ++it;
    ++count;
  }

  // for (const auto& entry : reader) {
  //   std::cout << "[" << entry.first << "] - "
  //            << std::string{reinterpret_cast<const
  //            char*>(entry.second.first),
  //                           entry.second.second}
  //            << "\n";
  //}
}