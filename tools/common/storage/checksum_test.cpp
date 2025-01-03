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

#include "tools/common/storage/checksum.hpp"

#include <gtest/gtest.h>

#include <vector>

using namespace inspector::tools::storage;

TEST(ChecksumTestFixture, TestChecksumNonNullData) {
  const std::vector<char> data = {'t', 'e', 's', 't', 'i', 'n', 'g', '_',
                                  'c', 'h', 'e', 'c', 'k', 's', 'u', 'm'};
  size_t value = checksum(data.data(), data.size());
  ASSERT_EQ(value, 956630705);
}

TEST(ChecksumTestFixture, TestChecksumForNullData) {
  const std::vector<uint8_t> data = {0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0};
  size_t value = checksum(data.data(), data.size());
  ASSERT_EQ(value, 1048577);
}
