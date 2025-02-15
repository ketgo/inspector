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

#include <cstdint>
#include <cstddef>

namespace inspector {
namespace tools {
namespace storage {

using ChecksumType = uint32_t;

/**
 * @brief Compute checksum for the given memory span.
 *
 * @param src Starting address of the memory span to compute checksum.
 * @param size Size in bytes of the memory span.
 * @returns CHecksum value.
 */
ChecksumType checksum(const void* src, const std::size_t size);

}  // namespace storage
}  // namespace tools
}  // namespace inspector