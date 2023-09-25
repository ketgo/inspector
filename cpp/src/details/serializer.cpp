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

#include <inspector/details/serializer.hpp>

#include <cstring>
#include <string>

namespace inspector {
namespace details {
namespace {

/**
 * @brief Type trait to get `DataType` enum value for an object type `T`.
 *
 * @tparam T Type of object.
 */
template <class T>
struct TypeId {};

// Template specializations for the different supported data types.

template <>
struct TypeId<int16_t> {
  static constexpr auto value = DataType::TYPE_INT16;
};

template <>
struct TypeId<int32_t> {
  static constexpr auto value = DataType::TYPE_INT32;
};

template <>
struct TypeId<int64_t> {
  static constexpr auto value = DataType::TYPE_INT64;
};

template <>
struct TypeId<uint8_t> {
  static constexpr auto value = DataType::TYPE_UINT8;
};

template <>
struct TypeId<uint16_t> {
  static constexpr auto value = DataType::TYPE_UINT16;
};

template <>
struct TypeId<uint32_t> {
  static constexpr auto value = DataType::TYPE_UINT32;
};

template <>
struct TypeId<uint64_t> {
  static constexpr auto value = DataType::TYPE_UINT64;
};

template <>
struct TypeId<float> {
  static constexpr auto value = DataType::TYPE_FLOAT;
};

template <>
struct TypeId<double> {
  static constexpr auto value = DataType::TYPE_DOUBLE;
};

template <>
struct TypeId<char> {
  static constexpr auto value = DataType::TYPE_CHAR;
};

template <>
struct TypeId<const char*> {
  static constexpr auto value = DataType::TYPE_CSTRING;
};

template <>
struct TypeId<std::string> {
  static constexpr auto value = DataType::TYPE_STRING;
};

}  // namespace

// ---
// `storageSize` Implementation
// ---

template <class T>
size_t storageSize(const T& obj) {
  // The first 8 bits store the type of data while the rest store the actual
  // data.
  return sizeof(uint8_t) + sizeof(obj);
}

// Instantiating for different supported data types
template size_t storageSize<int16_t>(const int16_t&);
template size_t storageSize<int32_t>(const int32_t&);
template size_t storageSize<int64_t>(const int64_t&);
template size_t storageSize<uint8_t>(const uint8_t&);
template size_t storageSize<uint16_t>(const uint16_t&);
template size_t storageSize<uint32_t>(const uint32_t&);
template size_t storageSize<uint64_t>(const uint64_t&);
template size_t storageSize<float>(const float&);
template size_t storageSize<double>(const double&);
template size_t storageSize<char>(const char&);

// Template specialization for string literal
template <>
size_t storageSize<const char*>(const char* const& obj) {
  // The size includes first 8 bits for type, followed by size for storing the
  // chars in the string including the null terminator.
  return sizeof(uint8_t) + std::strlen(obj) + sizeof(char);
}

// Template specialization for std::string
template <>
size_t storageSize<std::string>(const std::string& obj) {
  // The size includes first 8 bits for type, followed by size for storing
  // number of chars in the string, followed by the chars in the string.
  return sizeof(uint8_t) + sizeof(std::size_t) + obj.size() * sizeof(char);
}

// ---
// `dump` Implementation
// ---

template <class T>
void dump(void* address, const T& obj) {
  *static_cast<uint8_t*>(address) = static_cast<uint8_t>(TypeId<T>::value);
  std::memcpy(
      static_cast<void*>(static_cast<uint8_t*>(address) + sizeof(uint8_t)),
      static_cast<const void*>(&obj), sizeof(obj));
}

// Instantiating for different supported data types
template void dump<int16_t>(void*, const int16_t&);
template void dump<int32_t>(void*, const int32_t&);
template void dump<int64_t>(void*, const int64_t&);
template void dump<uint8_t>(void*, const uint8_t&);
template void dump<uint16_t>(void*, const uint16_t&);
template void dump<uint32_t>(void*, const uint32_t&);
template void dump<uint64_t>(void*, const uint64_t&);
template void dump<float>(void*, const float&);
template void dump<double>(void*, const double&);
template void dump<char>(void*, const char&);

// Template specialization for string literal
template <>
void dump<const char*>(void* address, const char* const& obj) {
  *static_cast<uint8_t*>(address) =
      static_cast<uint8_t>(TypeId<const char*>::value);
  std::memcpy(
      static_cast<void*>(static_cast<uint8_t*>(address) + sizeof(uint8_t)),
      static_cast<const void*>(obj), std::strlen(obj) + sizeof(char));
}

// Template specialization for string
template <>
void dump<std::string>(void* address, const std::string& obj) {
  *static_cast<uint8_t*>(address) =
      static_cast<uint8_t>(TypeId<std::string>::value);
  *static_cast<std::size_t*>(static_cast<void*>(static_cast<uint8_t*>(address) +
                                                sizeof(uint8_t))) = obj.size();
  std::memcpy(static_cast<void*>(static_cast<uint8_t*>(address) +
                                 sizeof(uint8_t) + sizeof(std::size_t)),
              static_cast<const void*>(obj.data()), obj.size());
}

// ---
// `load` Implementation
// ---

DataType dataType(const void* address) {
  return static_cast<DataType>(*static_cast<const uint8_t*>(address));
}

template <class T>
T load(const void* address) {
  if (dataType(address) != TypeId<T>::value) {
    throw std::runtime_error(
        "Invalid type '" + std::to_string(static_cast<int>(TypeId<T>::value)) +
        "' loaded.");
  }
  return *static_cast<const T*>(static_cast<const void*>(
      static_cast<const uint8_t*>(address) + sizeof(uint8_t)));
}

// Instantiating for different supported data types
template int16_t load<int16_t>(const void*);
template int32_t load<int32_t>(const void*);
template int64_t load<int64_t>(const void*);
template uint8_t load<uint8_t>(const void*);
template uint16_t load<uint16_t>(const void*);
template uint32_t load<uint32_t>(const void*);
template uint64_t load<uint64_t>(const void*);
template float load<float>(const void*);
template double load<double>(const void*);
template char load<char>(const void*);

// Template specialization for string literals
template <>
const char* load<const char*>(const void* address) {
  if (dataType(address) != TypeId<const char*>::value) {
    throw std::runtime_error(
        "Invalid type '" +
        std::to_string(static_cast<int>(TypeId<const char*>::value)) +
        "' loaded.");
  }

  return static_cast<const char*>(static_cast<const void*>(
      static_cast<const uint8_t*>(address) + sizeof(uint8_t)));
}

// Template specialization for strings
template <>
std::string load<std::string>(const void* address) {
  if (dataType(address) != TypeId<std::string>::value) {
    throw std::runtime_error(
        "Invalid type '" +
        std::to_string(static_cast<int>(TypeId<std::string>::value)) +
        "' loaded.");
  }

  std::size_t size = *static_cast<const std::size_t*>(static_cast<const void*>(
      static_cast<const uint8_t*>(address) + sizeof(uint8_t)));
  const char* data = static_cast<const char*>(
      static_cast<const void*>(static_cast<const uint8_t*>(address) +
                               sizeof(uint8_t) + sizeof(std::size_t)));

  return std::string{data, size};
}

}  // namespace details
}  // namespace inspector