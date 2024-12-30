/**
 * Copyright 2022 Ketan Goyal
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

namespace utils {

/**
 * @brief Get the current directory.
 *
 */
std::string currentDir();

/**
 * @brief The class `TempDir` represents a temporary directory and exposes
 * methods to perform file-system operations. The directory is temporary since
 * it only exists in the file-system during the lifetime of the class object.
 *
 */
class TempDir {
 public:
  /**
   * @brief Construct a new TempDir object.
   *
   * The CTOR creates the directory in the file-system.
   *
   * @param path Constant reference to the path of the directory.
   * @param remove Flag to remove the temporary directoy as part of DTOR.
   */
  explicit TempDir(const std::string& path, const bool remove = true);

  /**
   * @brief Destroy the TempDir object.
   *
   * The DTOR removes the directory and all the files it contains from the
   * file-system.
   */
  ~TempDir();

  /**
   * @brief Get the path to the directory.
   *
   * @returns Path to the directory as string.
   */
  const std::string& path() const;

  /**
   * @brief Copy a file to the directory.
   *
   * @param source_path Path to the file to copy.
   * @param destination_path Path to the copy destination within the temporay
   * directory.
   */
  void copyFile(const std::string& source_path,
                const std::string& destination_path) const;

  /**
   * @brief Check if a file exists within the directory.
   *
   * @param path Path to the file within the directory.
   * @returns `true` if exists else `false`.
   */
  bool fileExists(const std::string& path) const;

  /**
   * @brief Read a file located within the directory.
   *
   * @param path Path to the file within the directory.
   * @returns Data loaded as string.
   */
  std::string readFile(const std::string& path) const;

  /**
   * @brief Read a file located within the directory.
   *
   * @param path Path to the file within the directory.
   * @param offset Offset in the file from where to start reading.
   * @returns Data loaded as string.
   */
  std::string readFile(const std::string& path, const size_t offset) const;

  /**
   * @brief Read a file located within the directory.
   *
   * @param path Path to the file within the directory.
   * @param offset Offset in the file from where to start reading.
   * @param num Number of bytes to read.
   * @return std::string
   */
  std::string readFile(const std::string& path, const size_t offset,
                       const size_t num) const;

  /**
   * @brief List all files in the directory.
   *
   * @param recursive Flag to list files in subdirectories.
   * @returns List of file paths.
   */
  std::vector<std::string> listFiles(const bool recursive = false) const;

  /**
   * @brief Cast as string to get the path of the directory.
   *
   * @returns Path to the directory.
   */
  operator std::string() const;

 private:
  const std::string path_;
  const bool remove_;
};

}  // namespace utils