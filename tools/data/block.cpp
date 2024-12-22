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

#include "tools/data/block.hpp"

#include <cassert>
#include <cstring>
#include <limits>
#include <queue>
#include <vector>

namespace inspector {
namespace tools {
namespace data {
namespace {

/**
 * @brief Data structure representing a block header.
 */
struct PACKED BlockHeader {
  std::size_t count;
  timestamp_t start_timestamp;
  timestamp_t end_timestamp;
};

/**
 * @brief Data structure containing timestamp, offset and size of a record in
 * the block.
 */
struct PACKED RecordIndex {
  timestamp_t timestamp;
  std::size_t offset;
  std::size_t size;
};

/**
 * @brief Functor to compare timestamps of two record indecies. It is used to
 * chronologically order indecies.
 *
 */
class RecordIndexCompare {
 public:
  bool operator()(const RecordIndex& lhs, const RecordIndex& rhs) const {
    return lhs.timestamp >= rhs.timestamp;
  }
};

}  // namespace

// -------------------------------------------------------------
// BlockBuilder
// -------------------------------------------------------------

class BlockBuilder::Impl {
 public:
  explicit Impl(const std::size_t block_size)
      : buffer_(block_size), record_head_(), index_head_() {
    reset();
  }

  std::size_t count() const { return header().count; }

  timestamp_t startTimestamp() const { return header().start_timestamp; }

  timestamp_t endTimestamp() const { return header().end_timestamp; }

  bool add(const timestamp_t timestamp, const void* const src,
           const std::size_t size) {
    const auto storage_size = sizeof(RecordIndex) + size;
    // Checking for enough free space in the block.
    if (storage_size > freeSpace()) {
      return false;
    }

    // Copying data into body
    record_head_ -= size;
    std::memcpy(body() + record_head_, src, size);

    // Find location to insert the record index
    std::size_t insert_head = 0, step = 0, count = header().count;
    while (count > 0) {
      step = count / 2;
      insert_head = step * sizeof(RecordIndex);
      if (recordIndex(insert_head).timestamp < timestamp) {
        insert_head += sizeof(RecordIndex);
        count -= step + 1;
      } else {
        count = step;
      }
    }

    // Creating a new index record at the end and then swaping it to the correct
    // location.
    std::size_t head = index_head_;
    auto& index = recordIndex(head);
    index.timestamp = timestamp;
    index.size = size;
    index.offset = record_head_;
    index_head_ += sizeof(RecordIndex);
    while (head > insert_head) {
      const auto prev = head - sizeof(RecordIndex);
      std::swap(recordIndex(head), recordIndex(prev));
      head = prev;
    }

    // Updating header
    header().count += 1;
    header().start_timestamp = header().start_timestamp > timestamp
                                   ? timestamp
                                   : header().start_timestamp;
    header().end_timestamp =
        header().end_timestamp < timestamp ? timestamp : header().end_timestamp;

    return true;
  }

  void flush(const File& file) {
    std::memset(body() + index_head_, 0, freeSpace());
    file.write(buffer_.data(), buffer_.size(), 0);
    reset();
  }

 private:
  BlockHeader& header() {
    return *reinterpret_cast<BlockHeader*>(buffer_.data());
  }

  const BlockHeader& header() const {
    return *reinterpret_cast<const BlockHeader*>(buffer_.data());
  }

  uint8_t* body() { return buffer_.data() + sizeof(BlockHeader); }

  const uint8_t* body() const { return buffer_.data() + sizeof(BlockHeader); }

  RecordIndex& recordIndex(const std::size_t head) {
    return *reinterpret_cast<RecordIndex*>(body() + head);
  }

  const RecordIndex& recordIndex(const std::size_t head) const {
    return *reinterpret_cast<const RecordIndex*>(body() + head);
  }

  std::size_t freeSpace() const {
    assert(record_head_ >= index_head_);
    return record_head_ - index_head_;
  }

  void reset() {
    header().count = 0;
    header().start_timestamp = std::numeric_limits<timestamp_t>::max();
    header().end_timestamp = std::numeric_limits<timestamp_t>::min();
    // NOTE: Head values are offseted from body not start of buffer.
    index_head_ = 0;
    record_head_ = buffer_.size() - sizeof(BlockHeader);
  }

  std::vector<uint8_t> buffer_;
  std::size_t record_head_;
  std::size_t index_head_;
};

// ---

BlockBuilder::BlockBuilder(const std::size_t block_size)
    : impl_(std::make_shared<Impl>(block_size)) {}

std::size_t BlockBuilder::count() const { return impl_->count(); }

timestamp_t BlockBuilder::startTimestamp() const {
  return impl_->startTimestamp();
}

timestamp_t BlockBuilder::endTimestamp() const { return impl_->endTimestamp(); }

bool BlockBuilder::add(const timestamp_t timestamp, const void* const src,
                       const std::size_t size) {
  return impl_->add(timestamp, src, size);
}

void BlockBuilder::flush(const File& file) { impl_->flush(file); }

// -------------------------------------------------------------

// -------------------------------------------------------------
// BlockReader
// -------------------------------------------------------------

class BlockReader::Impl {
  friend class BlockReader::Iterator;

 public:
  explicit Impl(const File& file) : buffer_(file.size()) {
    file.read(buffer_.data(), buffer_.size(), 0);
  }

  std::size_t count() const { return header().count; }

  timestamp_t startTimestamp() const { return header().start_timestamp; }

  timestamp_t endTimestamp() const { return header().end_timestamp; }

 private:
  const BlockHeader& header() const {
    return *reinterpret_cast<const BlockHeader*>(buffer_.data());
  }

  const uint8_t* body() const { return buffer_.data() + sizeof(BlockHeader); }

  const RecordIndex& recordIndex(const std::size_t head) const {
    return *reinterpret_cast<const RecordIndex*>(body() + head);
  }

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