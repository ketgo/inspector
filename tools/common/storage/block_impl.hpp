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

#include "tools/common/storage/common.hpp"

namespace inspector {
namespace tools {
namespace storage {

/**
 * @brief Data structure representing a block header. It contains information on
 * the number of records in the block and indecies to the records in
 * chronological order.
 */
struct PACKED BlockHeader {
  std::size_t count;
};

/**
 * @brief Data structure represents an index to a record in a block. It contains
 * information on timestamp, offset and size of the record. Indecies are stored
 * right after the header in chronological order.
 */
struct PACKED RecordIndex {
  timestamp_t timestamp;
  std::size_t offset;
  std::size_t size;
};

}  // namespace storage
}  // namespace tools
}  // namespace inspector