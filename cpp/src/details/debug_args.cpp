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

#include <cstring>
#include <inspector/details/debug_args.hpp>
#include <string>

namespace inspector {
namespace details {

template <class T>
size_t debugArgStorageSize(const T &arg) {
  // The first 8 bits store the type of data while the rest store the actual
  // data.
  return sizeof(uint8_t) + sizeof(arg);
}

// Instantiating for different supported data types
template size_t debugArgStorageSize<int16_t>(const int16_t &);
template size_t debugArgStorageSize<int32_t>(const int32_t &);
template size_t debugArgStorageSize<int64_t>(const int64_t &);
template size_t debugArgStorageSize<uint8_t>(const uint8_t &);
template size_t debugArgStorageSize<uint16_t>(const uint16_t &);
template size_t debugArgStorageSize<uint32_t>(const uint32_t &);
template size_t debugArgStorageSize<uint64_t>(const uint64_t &);
template size_t debugArgStorageSize<float>(const float &);
template size_t debugArgStorageSize<double>(const double &);
template size_t debugArgStorageSize<char>(const char &);

// Template specialization for c-string
template <>
size_t debugArgStorageSize<const char *>(const char *const &obj) {
  // The size includes first 8 bits for type, followed by size for storing the
  // chars in the string including the null terminator.
  return sizeof(uint8_t) + std::strlen(obj) * sizeof(char) + sizeof(char);
}

// Template specialization for std::string
template <>
size_t debugArgStorageSize<std::string>(const std::string &obj) {
  return debugArgStorageSize(obj.c_str());
}

}  // namespace details
}  // namespace inspector