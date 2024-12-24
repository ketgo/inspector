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

#include <queue>
#include <string>
#include <vector>

#include "tools/common/storage/block_reader.hpp"
#include "tools/common/storage/common.hpp"

namespace inspector {
namespace tools {
namespace data {

class Reader {
 public:
  class Iterator {
    friend class Reader;

   public:
    using value_type =
        std::pair<timestamp_t, std::pair<const void*, std::size_t>>;

    Iterator() = default;
    Iterator& operator++();
    Iterator operator++(int);
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;
    const value_type* operator->() const;
    const value_type& operator*() const;
    std::size_t numBlocks() const;

   private:
    Iterator(const Reader& reader, const bool begin = true);

    void next();
    void createBlockReaders();
    bool updateValue();

    const Reader* reader_;

    struct _BlockReader {
      BlockReader reader;
      BlockReader::Iterator iterator;

      explicit _BlockReader(const BlockReader& _reader);
    };

    struct _BlockReaderCompare {
      bool operator()(const _BlockReader& lhs, const _BlockReader& rhs) const;
    };

    using BlockReaderQueue =
        std::priority_queue<_BlockReader, std::vector<_BlockReader>,
                            _BlockReaderCompare>;
    BlockReaderQueue queue_;
    std::size_t num_blocks_;
    value_type value_;
  };

  enum class ReadMode : uint8_t {
    kAlwaysChronological,
    kAlmostChronological,
  };

  Reader(const std::string& path, const std::size_t max_blocks = 1024,
         const ReadMode mode = ReadMode::kAlwaysChronological);

  Iterator begin() const;
  Iterator end() const;

 private:
  const std::string path_;
  const std::size_t max_blocks_;
  const ReadMode mode_;
};

}  // namespace data
}  // namespace tools
}  // namespace inspector