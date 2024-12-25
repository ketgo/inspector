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

#include <string>
#include <vector>

#include "tools/common/storage/common.hpp"
#include "tools/common/storage/file_io.hpp"

namespace inspector {
namespace tools {
namespace storage {

/**
 * @brief The class `BlockBuilder` exposes API to create a block of
 * chronologically sorted records.
 *
 */
class BlockBuilder {
 public:
  /**
   * @brief Construct a new BlockBuilder object.
   *
   * @param block_size Size in bytes of the block.
   */
  explicit BlockBuilder(const std::size_t block_size);

  /**
   * @brief Get the number of records in the block.
   *
   * @returns Number of records.
   */
  std::size_t count() const;

  /**
   * @brief Add the given record in the block.
   *
   * @param record Record to add into the block.
   * @returns On sucess `true` otherwise `false`. The method returns `false`
   * when not enough space is available to store the record.
   */
  bool add(const Record& record);

  /**
   * @brief Flush the contents in the block to the given file and reset the
   * block.
   *
   */
  void flush(const File& file);

 private:
  std::vector<uint8_t> buffer_;
};

/**
 * @brief The class `BlockReader` exposes API to read records stored in a block.
 * Reading of records is performed using iterators.
 *
 */
class BlockReader {
 public:
  /**
   * @brief Forward only iterator for reading records in a block.
   *
   */
  class Iterator {
    friend class BlockReader;

   public:
    Iterator() = default;
    Iterator& operator++();
    // NOTE: We do not implement the following for efficiency:
    // Iterator operator++(int);
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;
    const Record* operator->() const;
    const Record& operator*() const;

   private:
    Iterator(const BlockReader& reader, const std::size_t index);
    void next();

    const BlockReader* reader_;
    std::size_t index_;
    Record record_;
  };

  /**
   * @brief Construct a block reader.
   *
   * @param file File containing block.
   */
  explicit BlockReader(const File& file);

  /**
   * @brief Get the number of records in the block.
   *
   * @returns Number of records.
   */
  std::size_t count() const;

  /**
   * @brief Get iterator to the first record.
   *
   */
  Iterator begin() const;

  /**
   * @brief Get iterator to one past last record.
   *
   */
  Iterator end() const;

 private:
  std::string path_;
  std::vector<uint8_t> buffer_;
};

}  // namespace storage
}  // namespace tools
}  // namespace inspector