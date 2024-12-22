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

#include "tools/data/writer.hpp"

#include "tools/data/block.hpp"

namespace inspector {
namespace tools {
namespace data {

Writer::Writer(const std::string& path, const std::size_t block_size)
    : path_(path), block_size_(block_size), data_size_(sizeof(Block::Header)) {}

void Writer::write(const int64_t timestamp, std::vector<uint8_t>&& buffer) {
  const auto data_size = data_size_ + Block::storageSize(buffer);
  if (data_size > block_size_) {
    // TODO: Pop and create a block to save to file.
  }

  data_.emplace(timestamp, std::move(buffer));
  data_size_ = data_size;
}

}  // namespace data
}  // namespace tools
}  // namespace inspector