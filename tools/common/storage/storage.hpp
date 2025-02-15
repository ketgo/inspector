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

#include <memory>
#include <queue>
#include <string>

#include "tools/common/storage/block.hpp"
#include "tools/common/storage/common.hpp"

namespace inspector {
namespace tools {
namespace storage {

/**
 * @brief The class `Writer` exposes API to write records in chronological order
 * to disk.
 *
 */
class Writer final {
 public:
  /**
   * @brief Construct a new writer.
   *
   * @param path Path to output directory.
   * @param block_size Maximum size of each file stored in the output directory.
   *
   */
  Writer(const std::string& path, const std::size_t block_size);

  /**
   * @brief Destory writer object.
   *
   * As part of DTOR flush is called.
   */
  ~Writer();

  /**
   * @brief Write given record.
   *
   * @param record Record to write into storage.
   */
  void write(const Record& record);

  /**
   * @brief Flush all contents to disk.
   *
   */
  void flush();

 private:
  std::string path_;
  BlockBuilder builder_;
  std::size_t num_blocks_;
};

/**
 * @brief The class Reader exposes API to read records from storage.
 *
 */
class Reader {
 public:
  /**
   * @brief Different modes of reading records.
   *
   */
  enum class ReadMode : uint8_t {
    kAlwaysChronological,
    kAlmostChronological,
  };

  /**
   * @brief Forward only iterator for reading records in storage.
   *
   */
  class Iterator {
    friend class Reader;

   public:
    Iterator() = default;
    Iterator& operator++();
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;
    const Record* operator->() const;
    const Record& operator*() const;

   private:
    Iterator(const Reader& reader, bool end);

    void next();
    void updateQueue();
    bool updateRecord();

    struct BlockReaderWrapper {
      BlockReader reader;
      BlockReader::Iterator it;

      NO_COPY(BlockReaderWrapper);
      explicit BlockReaderWrapper(BlockReader&& _reader);
    };
    using BlockReaderWrapperPtr = std::shared_ptr<BlockReaderWrapper>;

    struct BlockReaderWrapperPtrCompare {
      bool operator()(const BlockReaderWrapperPtr& lhs,
                      const BlockReaderWrapperPtr& rhs) const;
    };
    using BlockReaderQueue =
        std::priority_queue<BlockReaderWrapperPtr,
                            std::vector<BlockReaderWrapperPtr>,
                            BlockReaderWrapperPtrCompare>;

    std::string path_;
    Reader::ReadMode mode_;
    std::size_t max_blocks_;
    std::size_t num_blocks_;
    BlockReaderQueue queue_;
    Record record_;
  };

  /**
   * @brief Construct a Reader object.
   *
   * @param path Path where storage is located.
   * @param max_blocks Maximum number of blocks to load at once.
   * @param mode Mode of reading records.
   */
  Reader(const std::string& path, const std::size_t max_blocks = 1024,
         const ReadMode mode = ReadMode::kAlwaysChronological);

  Iterator begin() const;
  Iterator end() const;

 private:
  const std::string path_;
  const std::size_t max_blocks_;
  const ReadMode mode_;
};

}  // namespace storage
}  // namespace tools
}  // namespace inspector