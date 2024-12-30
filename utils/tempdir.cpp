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

#include "utils/tempdir.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <exception>
#include <sstream>

namespace utils {

std::string currentDir() { return boost::filesystem::current_path().string(); }

// --------------------------
// TempDir
// --------------------------

TempDir::TempDir(const std::string& path, const bool remove)
    : path_(path), remove_(remove) {
  boost::filesystem::create_directories(path_);
}

TempDir::~TempDir() {
  if (remove_) {
    boost::filesystem::remove_all(path_);
  }
}

const std::string& TempDir::path() const { return path_; }

void TempDir::copyFile(const std::string& source_path,
                       const std::string& destination_path) const {
  auto source = boost::filesystem::canonical(source_path);
  auto destination = boost::filesystem::path(path_) /
                     boost::filesystem::path(destination_path);
  boost::filesystem::create_directory(destination.parent_path());
  boost::filesystem::copy(source, destination,
                          boost::filesystem::copy_options::overwrite_existing);
}

bool TempDir::fileExists(const std::string& path) const {
  return boost::filesystem::exists(boost::filesystem::path(path_) /
                                   boost::filesystem::path(path));
}

std::string TempDir::readFile(const std::string& path) const {
  boost::filesystem::ifstream f_stream(boost::filesystem::path(path_) /
                                       boost::filesystem::path(path));
  std::stringstream s_stream;
  s_stream << f_stream.rdbuf();
  return s_stream.str();
}

std::string TempDir::readFile(const std::string& path,
                              const size_t offset) const {
  boost::filesystem::ifstream f_stream(boost::filesystem::path(path_) /
                                       boost::filesystem::path(path));
  f_stream.seekg(offset);
  std::stringstream s_stream;
  s_stream << f_stream.rdbuf();
  return s_stream.str();
}

std::string TempDir::readFile(const std::string& path, const size_t offset,
                              const size_t num) const {
  boost::filesystem::ifstream f_stream(boost::filesystem::path(path_) /
                                       boost::filesystem::path(path));
  f_stream.seekg(offset);
  std::string data(num, ' ');
  for (size_t i = 0; i < num; ++i) {
    f_stream.get(data[i]);
  }
  return data;
}

std::vector<std::string> TempDir::listFiles(const bool recursive) const {
  std::vector<std::string> files;

  if (recursive) {
    for (auto& entry : boost::filesystem::recursive_directory_iterator(
             boost::filesystem::path(path_))) {
      if (!boost::filesystem::is_regular_file(entry)) {
        continue;
      }
      files.emplace_back(entry.path().string());
    }
  } else {
    for (auto& entry : boost::filesystem::directory_iterator(
             boost::filesystem::path(path_))) {
      files.emplace_back(entry.path().string());
    }
  }

  return files;
}

TempDir::operator std::string() const { return path(); }

// --------------------------

}  // namespace utils