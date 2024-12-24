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

#include "tools/common/storage/reader.hpp"

#include <iostream>

namespace inspector {
namespace tools {
namespace data {

Reader::Iterator::_BlockReader::_BlockReader(const BlockReader& _reader)
    : reader(_reader), iterator(reader.begin()) {}

bool Reader::Iterator::_BlockReaderCompare::operator()(
    const _BlockReader& lhs, const _BlockReader& rhs) const {
  return lhs.iterator->first > rhs.iterator->first;
}

// private
void Reader::Iterator::next() {
  // Do nothing if we are already at the end.
  if (queue_.empty()) {
    return;
  }

  std::cout << "POP: " << queue_.top().iterator->first << "\n";
  ++(const_cast<_BlockReader&>(queue_.top()).iterator);
  std::cout << "POP: " << queue_.top().iterator->first << "\n";

  do {
    /*
    _BlockReader block_reader = queue_.top();
    queue_.pop();
    std::cout << "POP: " << block_reader.iterator->first << "\n";
    ++block_reader.iterator;
    if (block_reader.iterator == block_reader.reader.end()) {
      createBlockReaders();
    } else {
      std::cout << "EMPLACE: " << block_reader.iterator->first << "\n";
      queue_.emplace(block_reader);
      std::cout << "TOP: " << queue_.top().iterator->first << "\n";
    }
    */
  } while (!updateValue());
}

// private
void Reader::Iterator::createBlockReaders() {
  while (queue_.size() < reader_->max_blocks_) {
    const auto file_name = std::to_string(num_blocks_) + kFileExtension;
    if (!File::exists(file_name, reader_->path_)) {
      return;
    }
    BlockReader block_reader{File{file_name, reader_->path_}};
    if (block_reader.count()) {
      queue_.emplace(block_reader);
    }
    ++num_blocks_;
  }
}

// private
bool Reader::Iterator::updateValue() {
  // if (value_.first > queue_.top().iterator->first &&
  //     reader_->mode_ == Reader::ReadMode::kAlwaysChronological) {
  //   return false;
  // }
  value_ = *(queue_.top().iterator);
  return true;
}

// private
Reader::Iterator::Iterator(const Reader& reader, const bool begin)
    : reader_(std::addressof(reader)), num_blocks_(0) {
  if (begin) {
    createBlockReaders();
    if (!queue_.empty()) {
      updateValue();
    }
  }
}

Reader::Iterator& Reader::Iterator::operator++() {
  next();
  return *this;
}

Reader::Iterator Reader::Iterator::operator++(int) {
  Iterator it = *this;
  next();

  return it;
}

bool Reader::Iterator::operator==(const Iterator& other) const {
  return reader_ && other.reader_ && reader_->path_ == other.reader_->path_ &&
         queue_.size() == other.queue_.size() &&
         (queue_.empty()
              ? true
              : queue_.top().iterator == other.queue_.top().iterator);
}

bool Reader::Iterator::operator!=(const Iterator& other) const {
  return !(*this == other);
}

const Reader::Iterator::value_type* Reader::Iterator::operator->() const {
  return std::addressof(value_);
}

const Reader::Iterator::value_type& Reader::Iterator::operator*() const {
  return value_;
}

std::size_t Reader::Iterator::numBlocks() const { return num_blocks_; }

Reader::Reader(const std::string& path, const std::size_t max_blocks,
               const ReadMode mode)
    : path_(path), max_blocks_(max_blocks), mode_(mode) {}

Reader::Iterator Reader::begin() const { return Iterator(*this, true); }

Reader::Iterator Reader::end() const { return Iterator(*this, false); }

}  // namespace data
}  // namespace tools
}  // namespace inspector