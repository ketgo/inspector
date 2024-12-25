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

#include "tools/common/storage/checksum.hpp"

namespace inspector {
namespace tools {
namespace storage {
namespace {

constexpr uint32_t kMod = 65521;

}

uint32_t checksum(const void* src, const std::size_t size) {
  uint32_t a = 1, b = 0;

  std::size_t count = 0;
  auto it = static_cast<const uint8_t*>(src);
  while (count < size) {
    a = (a + *it) % kMod;
    b = (b + a) % kMod;
    ++count;
    ++it;
  }

  return (b << 16) | a;
}

}  // namespace storage
}  // namespace tools
}  // namespace inspector
