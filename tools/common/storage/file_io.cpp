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

#include "tools/common/storage/file_io.hpp"

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <system_error>

namespace inspector {
namespace tools {
namespace storage {
namespace {

/**
 * @brief Null file descriptor.
 *
 */
constexpr int kNullFileDescriptor = -1;

/**
 * @brief Open a file having given name and path.
 *
 * @param name Constant reference to the file name.
 * @param path Constant reference to the path to the file excluding file name.
 * @returns File descriptor.
 */
int openFile(const std::string& name, const std::string& path) {
  auto status = ::mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (!status && status != EEXIST) {
    throw std::system_error(
        errno, std::generic_category(),
        "Error calling 'mkdir' for directory '" + path + "': ");
  }
  const auto file = path + "/" + name;
  auto fd = ::open(file.c_str(), O_CREAT | O_RDWR,
                   S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (fd < 0) {
    throw std::system_error(errno, std::generic_category(),
                            "Error calling 'open' for file '" + file + "': ");
  }
  return fd;
}

}  // namespace

// static
bool File::exists(const std::string& name, const std::string& path) {
  const std::string file = path + "/" + name;

  struct stat buffer;
  return ::stat(file.c_str(), &buffer) == 0;
}

File::File(const std::string& name, const std::string& path)
    : path_(path + "/" + name), fd_(openFile(name, path)) {}

File::~File() {
  if (fd_ != kNullFileDescriptor) {
    ::close(fd_);
  }
}

File::File(File&& other) : path_(std::move(other.path_)), fd_(other.fd_) {
  other.fd_ = kNullFileDescriptor;
}

File& File::operator=(File&& other) {
  if (this != &other) {
    path_ = std::move(other.path_);
    fd_ = other.fd_;
    other.fd_ = kNullFileDescriptor;
  }
  return *this;
}

std::size_t File::read(void* const dest, const std::size_t size,
                       const std::size_t offset) const {
  auto bytes = ::pread(fd_, dest, size, offset);
  if (bytes < 0) {
    throw std::system_error(errno, std::generic_category());
  }
  return bytes;
}

std::size_t File::write(const void* const src, const std::size_t size,
                        const std::size_t offset) const {
  const auto bytes = ::pwrite(fd_, src, size, offset);
  if (bytes < 0) {
    throw std::system_error(errno, std::generic_category());
  }
  return bytes;
}

void File::sync() const {
  if (::fsync(fd_) == -1) {
    throw std::system_error(errno, std::generic_category());
  }
}

void File::resize(const std::size_t size) const {
  if (::ftruncate(fd_, size) == -1) {
    throw std::system_error(errno, std::generic_category());
  }
}

std::size_t File::size() const {
  struct stat buf;
  if (::fstat(fd_, &buf) == -1) {
    throw std::system_error(errno, std::generic_category());
  }
  return buf.st_size;
}

void File::remove() const {
  if (::close(fd_) == -1) {
    throw std::system_error(errno, std::generic_category());
  }
  if (::remove(path_.c_str()) == -1) {
    throw std::system_error(errno, std::generic_category());
  }
}

const std::string& File::path() const { return path_; }

}  // namespace storage
}  // namespace tools
}  // namespace inspector