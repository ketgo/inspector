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

#include "tools/data/block_reader.hpp"

#include <cassert>
#include <cstring>
#include <limits>
#include <queue>
#include <vector>

#include "tools/data/block_impl.hpp"

namespace inspector {
namespace tools {
namespace data {

// -------------------------------------------------------------
// BlockReader
// -------------------------------------------------------------

class BlockReader::Impl {
 public:
  explicit Impl(const File& file) : buffer_(file.size()) {
    file.read(buffer_.data(), buffer_.size(), 0);
  }

  std::size_t count() const { return header().count; }

  timestamp_t startTimestamp() const { return header().start_timestamp; }

  timestamp_t endTimestamp() const { return header().end_timestamp; }

  const BlockHeader& header() const {
    return *reinterpret_cast<const BlockHeader*>(buffer_.data());
  }

  const uint8_t* body() const { return buffer_.data() + sizeof(BlockHeader); }

  const RecordIndex& recordIndex(const std::size_t head) const {
    return *reinterpret_cast<const RecordIndex*>(body() + head);
  }

 private:
  std::vector<uint8_t> buffer_;
};

// ---

// private
void BlockReader::Iterator::next() {
  // Do nothing if we are already at the end.
  if (index_ == reader_->count()) {
    return;
  }
  ++index_;
  updateValue();
}

// private
void BlockReader::Iterator::updateValue() {
  const auto& record_index =
      reader_->impl_->recordIndex(index_ * sizeof(RecordIndex));
  value_.first = record_index.timestamp;
  value_.second.first = reader_->impl_->body() + record_index.offset;
  value_.second.second = record_index.size;
}

// private
BlockReader::Iterator::Iterator(const BlockReader& reader,
                                const std::size_t index)
    : reader_(std::addressof(reader)), index_(index) {
  updateValue();
}

BlockReader::Iterator& BlockReader::Iterator::operator++() {
  next();
  return *this;
}

BlockReader::Iterator BlockReader::Iterator::operator++(int) {
  Iterator it = *this;
  next();

  return it;
}

bool BlockReader::Iterator::operator==(const Iterator& other) const {
  return index_ == other.index_ && reader_ == other.reader_;
}

bool BlockReader::Iterator::operator!=(const Iterator& other) const {
  return !(*this == other);
}

const BlockReader::Iterator::value_type* BlockReader::Iterator::operator->()
    const {
  return std::addressof(value_);
}

const BlockReader::Iterator::value_type& BlockReader::Iterator::operator*()
    const {
  return value_;
}

BlockReader::BlockReader(const File& file)
    : impl_(std::make_shared<Impl>(file)) {}

std::size_t BlockReader::count() const { return impl_->count(); }

timestamp_t BlockReader::startTimestamp() const {
  return impl_->startTimestamp();
}

timestamp_t BlockReader::endTimestamp() const { return impl_->endTimestamp(); }

BlockReader::Iterator BlockReader::begin() const { return Iterator(*this, 0); }

BlockReader::Iterator BlockReader::end() const {
  return Iterator(*this, impl_->count());
}

// -------------------------------------------------------------

}  // namespace data
}  // namespace tools
}  // namespace inspector