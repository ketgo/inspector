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

#include "tools/common/storage/writer.hpp"

#include <cassert>

#include "tools/common/storage/block_builder.hpp"

namespace inspector {
namespace tools {
namespace data {

class Writer::Impl {
 public:
  Impl(const std::string& path, const std::size_t block_size)
      : path_(path), builder_(block_size), num_blocks_(0) {}

  ~Impl() { flush(); }

  void write(const timestamp_t timestamp, const void* const src,
             const std::size_t size) {
    if (builder_.add(timestamp, src, size)) {
      return;
    }

    flush();
    assert(builder_.count() == 0);
    builder_.add(timestamp, src, size);
  }

  void flush() {
    if (builder_.count() == 0) {
      return;
    }

    File file(std::to_string(num_blocks_) + kFileExtension, path_);
    builder_.flush(file);
    file.sync();
    ++num_blocks_;
  }

 private:
  const std::string& path_;
  BlockBuilder builder_;
  std::size_t num_blocks_;
};

Writer::Writer(const std::string& path, const std::size_t block_size)
    : impl_(std::make_shared<Impl>(path, block_size)) {}

Writer::~Writer() { flush(); }

void Writer::write(const timestamp_t timestamp, const void* const src,
                   const std::size_t size) {
  impl_->write(timestamp, src, size);
}

void Writer::flush() { impl_->flush(); }

}  // namespace data
}  // namespace tools
}  // namespace inspector