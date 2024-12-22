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

#include "tools/data/common.hpp"

namespace inspector {
namespace tools {
namespace data {

/**
 * @brief The class `File` represents a block file on disk and can be used to
 * perform IO operations. It acts as an abstraction over the lower level OS
 * dependent APIs.
 *
 * @note The class is moveable only.
 *
 */
class File final {
 public:
  NO_COPY(File);

  /**
   * @brief Construct a new File object.
   *
   * The file is opened as part of the CTOR.
   *
   * @param name Constant reference to the file name.
   * @param path Constant reference to the path to the file excluding file name.
   */
  File(const std::string& name, const std::string& path = ".");

  /**
   * @brief Destroy the File object.
   *
   * The file is closed as part of the DTOR.
   *
   */
  ~File();

  /**
   * @brief Construct a new File object. Move CTOR.
   *
   */
  File(File&& other);

  /**
   * @brief Move assignment operator.
   *
   */
  File& operator=(File&& other);

  /**
   * @brief Read data from file to the memory block span.
   *
   * @param dest Loaction into which to read data.
   * @param size Number of bytes to read.
   * @param offset Offset in the file from where to read.
   * @returns Number of bytes read.
   */
  std::size_t read(void* const dest, const std::size_t size,
                   const std::size_t offset) const;

  /**
   * @brief Write data to file from the memory block represented by the given
   * span.
   *
   * @param src Location from which to write data to file.
   * @param size Number of bytes to write.
   * @param offset Offset in the file from where to write.
   * @returns Number of bytes written.
   */
  std::size_t write(const void* const src, const std::size_t size,
                    const std::size_t offset) const;

  /**
   * @brief The method writes all in-memory buffers of the kernel to disk. Thus
   * calling this method ensures that any write operations are immediately
   * written to the disk.
   *
   */
  void sync() const;

  /**
   * @brief Resize the file to given number of bytes.
   *
   * @param size Requested size of the file.
   */
  void resize(const size_t size) const;

  /**
   * @brief Get the size of file in bytes.
   *
   * @returns Size of file in bytes.
   */
  std::size_t size() const;

  /**
   * @brief Close and remove the file.
   *
   */
  void remove() const;

 private:
  std::string path_;
  int fd_;
};

}  // namespace data
}  // namespace tools
}  // namespace inspector