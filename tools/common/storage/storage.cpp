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

#include <cassert>

namespace inspector {
namespace tools {
namespace storage {

// ------------------------------------------------
// Writer
// ------------------------------------------------

Writer::Writer(const std::string& path, const std::size_t block_size)
    : path_(path), builder_(block_size), num_blocks_(0) {}

Writer::~Writer() { flush(); }

void Writer::write(const Record& record) {
  if (!builder_.add(record)) {
    flush();
    assert(builder_.count() == 0);
    builder_.add(record);
  }
}

void Writer::flush() {
  if (builder_.count() != 0) {
    File file(std::to_string(num_blocks_) + kFileExtension, path_);
    builder_.flush(file);
    ++num_blocks_;
  }
}

// ------------------------------------------------
// Reader
// ------------------------------------------------

Reader::Iterator::BlockReaderWrapper::BlockReaderWrapper(BlockReader&& _reader)
    : reader(std::move(_reader)), it(reader.begin()) {}

bool Reader::Iterator::BlockReaderWrapperPtrCompare::operator()(
    const BlockReaderWrapperPtr& lhs, const BlockReaderWrapperPtr& rhs) const {
  return lhs->it->timestamp >= rhs->it->timestamp;
}

// private
Reader::Iterator::Iterator(const Reader& reader, bool end)
    : path_(reader.path_),
      mode_(reader.mode_),
      max_blocks_(reader.max_blocks_),
      num_blocks_(0) {
  if (!end) {
    updateQueue();
    updateRecord();
  }
}

// private
void Reader::Iterator::next() {
  if (queue_.empty()) {
    record_ = {};
    return;
  }

  do {
    BlockReaderWrapperPtr block_reader = queue_.top();
    queue_.pop();
    ++(block_reader->it);
    if (block_reader->it == block_reader->reader.end()) {
      updateQueue();
    } else {
      queue_.emplace(std::move(block_reader));
    }
  } while (!updateRecord());
}

// private
void Reader::Iterator::updateQueue() {
  while (queue_.size() < max_blocks_) {
    const auto file_name = std::to_string(num_blocks_) + kFileExtension;
    if (!File::exists(file_name, path_)) {
      return;
    }
    BlockReader block_reader{File{file_name, path_}};
    if (block_reader.count()) {
      queue_.emplace(
          std::make_shared<BlockReaderWrapper>(std::move(block_reader)));
    }
    ++num_blocks_;
  }
}

bool Reader::Iterator::updateRecord() {
  if (queue_.empty()) {
    record_ = {};
    return true;
  }

  const auto timestamp = record_.timestamp;
  record_ = *(queue_.top()->it);
  return mode_ == ReadMode::kAlwaysChronological
             ? record_.timestamp >= timestamp
             : true;
}

Reader::Iterator& Reader::Iterator::operator++() {
  next();
  return *this;
}

bool Reader::Iterator::operator==(const Iterator& other) const {
  return path_ == other.path_ && mode_ == other.mode_ &&
         queue_.size() == other.queue_.size() &&
         (queue_.empty() ? true : queue_.top()->it == other.queue_.top()->it);
}

bool Reader::Iterator::operator!=(const Iterator& other) const {
  return !(*this == other);
}

const Record* Reader::Iterator::operator->() const {
  return std::addressof(record_);
}

const Record& Reader::Iterator::operator*() const { return record_; }

Reader::Reader(const std::string& path, const std::size_t max_blocks,
               const ReadMode mode)
    : path_(path), max_blocks_(max_blocks), mode_(mode) {}

Reader::Iterator Reader::begin() const { return Iterator{*this, false}; }

Reader::Iterator Reader::end() const { return Iterator{*this, true}; }

}  // namespace storage
}  // namespace tools
}  // namespace inspector