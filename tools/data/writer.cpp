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

#include <cassert>

#include "tools/data/block.hpp"

namespace inspector {
namespace tools {
namespace data {

Writer::Writer(const std::string& path, const std::size_t block_size)
    : path_(path), builder_(block_size), num_blocks_(0) {}

Writer::~Writer() { flush(); }

void Writer::write(const timestamp_t timestamp, const void* const src,
                   const std::size_t size) {
  if (builder_.add(timestamp, src, size)) {
    return;
  }

  flush();
  assert(builder_.count() == 0);
  builder_.add(timestamp, src, size);
}

void Writer::flush() {
  if (builder_.count() == 0) {
    return;
  }

  File file(std::to_string(num_blocks_), path_);
  builder_.flush(file);
  file.sync();
  ++num_blocks_;
}

}  // namespace data
}  // namespace tools
}  // namespace inspector