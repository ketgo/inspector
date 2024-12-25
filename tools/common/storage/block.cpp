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

#include "tools/common/storage/block.hpp"

#include <cassert>
#include <cstring>

#include "tools/common/storage/checksum.hpp"

namespace inspector {
namespace tools {
namespace storage {
namespace {

/**
 * @brief Data structure representing a block header. It contains the checksum,
 * number of records, and head (offset in bytes from start of body) to the free
 * space in the block.
 */
struct PACKED BlockHeader {
  ChecksumType checksum;
  std::size_t count;
  std::size_t fs_head;
};

/**
 * @brief Data structure representing an index to a record in a block. It
 * contains information on timestamp, offset and size of the record. Indecies
 * are stored right after the header in chronological order.
 */
struct PACKED RecordIndex {
  timestamp_t timestamp;
  std::size_t offset;
  std::size_t size;
};

/**
 * @brief The class exposes a read and write view of a block.
 *
 */
class BlockView {
 public:
  explicit BlockView(std::vector<uint8_t>& buffer) : buffer_(buffer) {}

  void reset() {
    header().count = 0;
    header().fs_head = buffer_.size() - sizeof(BlockHeader);
  }

  bool isCorrupt() const { return header().checksum != getChecksum(); }

  void setChecksum() { header().checksum = getChecksum(); }

  std::size_t count() const { return header().count; }

  bool insert(const Record& record) {
    // Checking for enough free space
    if (record.size + sizeof(RecordIndex) > freeSpace()) {
      return false;
    }

    // Copying data into body
    header().fs_head -= record.size;
    std::memcpy(body() + header().fs_head, record.src, record.size);

    // Find location to insert the record index
    std::size_t insert_idx = 0, idx = 0, step = 0, count = header().count;
    while (count > 0) {
      idx = insert_idx;
      step = count / 2;
      idx += step;
      if (recordIndex(idx).timestamp < record.timestamp) {
        insert_idx = idx + 1;
        count -= step + 1;
      } else {
        count = step;
      }
    }

    // Creating a new index record at the end and then swaping it to the correct
    // location.
    auto& record_index = recordIndex(header().count);
    record_index.timestamp = record.timestamp;
    record_index.size = record.size;
    record_index.offset = header().fs_head;
    idx = header().count;
    while (idx > insert_idx) {
      const auto prev_idx = idx - 1;
      std::swap(recordIndex(idx), recordIndex(prev_idx));
      idx = prev_idx;
    }
    header().count += 1;

    return true;
  }

  Record operator[](const std::size_t index) const {
    auto& record_index = recordIndex(index);
    return Record{record_index.timestamp, body() + record_index.offset,
                  record_index.size};
  }

  void flush(const File& file) {
    std::memset(body() + header().count * sizeof(RecordIndex), 0, freeSpace());
    file.write(buffer_.data(), buffer_.size(), 0);
    file.sync();
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

  RecordIndex& recordIndex(const std::size_t index) {
    return *reinterpret_cast<RecordIndex*>(body() +
                                           sizeof(RecordIndex) * index);
  }

  const RecordIndex& recordIndex(const std::size_t index) const {
    return *reinterpret_cast<const RecordIndex*>(body() +
                                                 sizeof(RecordIndex) * index);
  }

  ChecksumType getChecksum() const {
    return checksum(buffer_.data() + sizeof(ChecksumType),
                    buffer_.size() - sizeof(ChecksumType));
  }

  std::size_t freeSpace() const {
    const std::size_t index_head = header().count * sizeof(RecordIndex);
    assert(header().fs_head >= index_head);
    return header().fs_head - index_head;
  }

  std::vector<uint8_t>& buffer_;
};

/**
 * @brief The class exposes a read only view of a block.
 *
 */
class ConstBlockView {
 public:
  explicit ConstBlockView(const std::vector<uint8_t>& buffer)
      : buffer_(buffer) {}

  bool isCorrupt() const { return header().checksum != getChecksum(); }

  std::size_t count() const { return header().count; }

  Record operator[](const std::size_t index) const {
    auto& record_index = recordIndex(index);
    return Record{record_index.timestamp, body() + record_index.offset,
                  record_index.size};
  }

 private:
  const BlockHeader& header() const {
    return *reinterpret_cast<const BlockHeader*>(buffer_.data());
  }

  const uint8_t* body() const { return buffer_.data() + sizeof(BlockHeader); }

  const RecordIndex& recordIndex(const std::size_t index) const {
    return *reinterpret_cast<const RecordIndex*>(body() +
                                                 sizeof(RecordIndex) * index);
  }

  ChecksumType getChecksum() const {
    return checksum(buffer_.data() + sizeof(ChecksumType),
                    buffer_.size() - sizeof(ChecksumType));
  }

  const std::vector<uint8_t>& buffer_;
};

}  // namespace

// ----------------------------------------------------------------
// BlockBuilder
// ----------------------------------------------------------------

BlockBuilder::BlockBuilder(const std::size_t block_size) : buffer_(block_size) {
  BlockView(buffer_).reset();
}

std::size_t BlockBuilder::count() const {
  return ConstBlockView(buffer_).count();
}

bool BlockBuilder::add(const Record& record) {
  return BlockView(buffer_).insert(record);
}

void BlockBuilder::flush(const File& file) { BlockView(buffer_).flush(file); }

// ----------------------------------------------------------------
// BlockReader
// ----------------------------------------------------------------

// private
void BlockReader::Iterator::next() {
  assert(reader_);
  if (index_ == reader_->count()) {
    record_ = {};
    return;
  }
  ++index_;
  record_ = ConstBlockView(reader_->buffer_)[index_];
}

// private
BlockReader::Iterator::Iterator(const BlockReader& reader,
                                const std::size_t index)
    : reader_(std::addressof(reader)), index_(index) {
  if (index_ < reader_->count()) {
    record_ = ConstBlockView(reader_->buffer_)[index_];
  }
}

BlockReader::Iterator& BlockReader::Iterator::operator++() {
  next();
  return *this;
}

bool BlockReader::Iterator::operator==(const Iterator& other) const {
  return index_ == other.index_ && reader_ && other.reader_ &&
         reader_->path_ == other.reader_->path_;
}

bool BlockReader::Iterator::operator!=(const Iterator& other) const {
  return !(*this == other);
}

const Record* BlockReader::Iterator::operator->() const {
  return std::addressof(record_);
}

const Record& BlockReader::Iterator::operator*() const { return record_; }

BlockReader::BlockReader(const File& file)
    : path_(file.path()), buffer_(file.size()) {
  file.read(buffer_.data(), buffer_.size(), 0);
}

std::size_t BlockReader::count() const {
  return ConstBlockView(buffer_).count();
}

BlockReader::Iterator BlockReader::begin() const { return Iterator(*this, 0); }

BlockReader::Iterator BlockReader::end() const {
  return Iterator(*this, count());
}

}  // namespace storage
}  // namespace tools
}  // namespace inspector