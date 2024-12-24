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
#include <string>

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
   * @param timestamp Timestamp assocaited with the record.
   * @param src, size Location and size of the record.
   * @returns On sucess `true` otherwise `false`. The method returns `false`
   * when not enough space is available to store the record.
   */
  bool add(const timestamp_t timestamp, const void* const src,
           const std::size_t size);

  /**
   * @brief Flush the contents in the block to the given file and reset the
   * block.
   *
   */
  void flush(const File& file);

  // ---

 private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace storage
}  // namespace tools
}  // namespace inspector