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

namespace inspector {
namespace tools {
namespace data {

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
   * @param block_size Maximum size of each file stored in that directoy.
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
   * @param timestamp Timestamp associated with the record.
   * @param src Starting memory address of the record.
   * @param size Size of the record in bytes.
   */
  void write(const timestamp_t timestamp, const void* const src,
             const std::size_t size);

  /**
   * @brief Flush all contents to disk.
   *
   */
  void flush();

 private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace data
}  // namespace tools
}  // namespace inspector