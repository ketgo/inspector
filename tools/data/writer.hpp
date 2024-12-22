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

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "tools/data/common.hpp"

namespace inspector {
namespace tools {
namespace data {

/**
 * @brief The class `Writer` exposes API to write records in chronological order
 * to disk.
 *
 */
class Writer final {
 public:
  Writer(const Writer& other) = delete;
  Writer& operator=(const Writer& other) = delete;

  /**
   * @brief Construct a new writer.
   *
   * @param path Path to output directory.
   * @param block_size Maximum size of each file stored in that directoy.
   *
   */
  Writer(const std::string& path, const std::size_t block_size);

  void write(const int64_t timestamp, std::vector<uint8_t>&& buffer);

  void flush();

 private:
  const std::size_t block_size_;
  const std::string& path_;

  std::size_t data_size_;
  ChronologicalPriorityQueue<std::vector<uint8_t>> data_;
};

}  // namespace data
}  // namespace tools
}  // namespace inspector