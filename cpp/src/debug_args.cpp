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
#include <inspector/debug_args.hpp>
#include <inspector/details/debug_args.hpp>
#include <stdexcept>
#include <string>

#include "cpp/src/details/type_traits.hpp"

namespace inspector {

// ---
// `DebugArg` Implementation
// ---

DebugArg::DebugArg(const void *const address) : address_(address) {}

DebugArg::Type DebugArg::type() const {
  return static_cast<const Type>(*static_cast<const uint8_t *>(address_));
}

template <class T>
T DebugArg::value() const {
  if (type() != details::TypeId<T>::value) {
    throw std::runtime_error("Invalid type specified for argument of type '" +
                             std::to_string(static_cast<int>(type())) + "'.");
  }
  return *static_cast<const T *>(static_cast<const void *>(
      static_cast<const uint8_t *>(address_) + sizeof(uint8_t)));
}

// Instantiating for different supported data types
template int16_t DebugArg::value<int16_t>() const;
template int32_t DebugArg::value<int32_t>() const;
template int64_t DebugArg::value<int64_t>() const;
template uint8_t DebugArg::value<uint8_t>() const;
template uint16_t DebugArg::value<uint16_t>() const;
template uint32_t DebugArg::value<uint32_t>() const;
template uint64_t DebugArg::value<uint64_t>() const;
template float DebugArg::value<float>() const;
template double DebugArg::value<double>() const;
template char DebugArg::value<char>() const;

// Template specialization for c-string
template <>
const char *DebugArg::value<const char *>() const {
  if (type() != details::TypeId<const char *>::value) {
    throw std::runtime_error("Invalid type specified for argument of type '" +
                             std::to_string(static_cast<int>(type())) + "'.");
  }
  return static_cast<const char *>(static_cast<const void *>(
      static_cast<const uint8_t *>(address_) + sizeof(uint8_t)));
}

// Template specialization for std::string
template <>
std::string DebugArg::value<std::string>() const {
  return std::string{value<const char *>()};
}

// Template specialization for KeywordArg
template <>
KeywordArg DebugArg::value<KeywordArg>() const {
  if (type() != details::TypeId<KeywordArg>::value) {
    throw std::runtime_error("Invalid type specified for argument of type '" +
                             std::to_string(static_cast<int>(type())) + "'.");
  }
  return KeywordArg(static_cast<const void *>(
      static_cast<const uint8_t *>(address_) + sizeof(uint8_t)));
}

const void *DebugArg::address() const { return address_; }

// ---
// `KeywordArg` Implementation
// ---

KeywordArg::KeywordArg(const void *const address)
    : DebugArg(nullptr), name_(nullptr) {
  name_ = static_cast<const char *>(address);
  address_ = static_cast<const void *>(static_cast<const uint8_t *>(address) +
                                       std::strlen(name_) + sizeof(char));
}

const char *KeywordArg::name() const { return name_; }

// ---
// `DebugArgs::Iterator` Implementation
// ---

namespace {

/**
 * @brief Get the storage size in bytes occupied by the given debug argument.
 *
 */
size_t storageSize(const DebugArg &debug_arg) {
  switch (debug_arg.type()) {
    case DebugArg::Type::TYPE_INT16: {
      return details::debugArgStorageSize(debug_arg.value<int16_t>());
    }
    case DebugArg::Type::TYPE_INT32: {
      return details::debugArgStorageSize(debug_arg.value<int32_t>());
    }
    case DebugArg::Type::TYPE_INT64: {
      return details::debugArgStorageSize(debug_arg.value<int64_t>());
    }
    case DebugArg::Type::TYPE_UINT8: {
      return details::debugArgStorageSize(debug_arg.value<uint8_t>());
    }
    case DebugArg::Type::TYPE_UINT16: {
      return details::debugArgStorageSize(debug_arg.value<uint16_t>());
    }
    case DebugArg::Type::TYPE_UINT32: {
      return details::debugArgStorageSize(debug_arg.value<uint32_t>());
    }
    case DebugArg::Type::TYPE_UINT64: {
      return details::debugArgStorageSize(debug_arg.value<uint64_t>());
    }
    case DebugArg::Type::TYPE_FLOAT: {
      return details::debugArgStorageSize(debug_arg.value<float>());
    }
    case DebugArg::Type::TYPE_DOUBLE: {
      return details::debugArgStorageSize(debug_arg.value<double>());
    }
    case DebugArg::Type::TYPE_CHAR: {
      return details::debugArgStorageSize(debug_arg.value<char>());
    }
    case DebugArg::Type::TYPE_STRING: {
      return details::debugArgStorageSize(debug_arg.value<const char *>());
    }
    case DebugArg::Type::TYPE_KWARG: {
      const auto arg = debug_arg.value<KeywordArg>();
      return details::debugArgStorageSize(arg.name()) + storageSize(arg);
    }
  }

  return 0;
}

}  // namespace

DebugArgs::Iterator::Iterator() : debug_arg_(nullptr), count_(0) {}

DebugArgs::Iterator::Iterator(const void *const address, const size_t count)
    : debug_arg_(address), count_(count) {}

const DebugArg &DebugArgs::Iterator::operator*() const { return debug_arg_; }

const DebugArg *DebugArgs::Iterator::operator->() const {
  return std::addressof(debug_arg_);
}

DebugArgs::Iterator &DebugArgs::Iterator::operator++() {
  if (count_ == 0) {
    return *this;
  }
  debug_arg_ = DebugArg(static_cast<const void *>(
      static_cast<const uint8_t *>(debug_arg_.address()) +
      storageSize(debug_arg_)));
  --count_;
  return *this;
}

bool DebugArgs::Iterator::operator==(const Iterator &other) const {
  return debug_arg_.address() == other.debug_arg_.address() &&
         count_ == other.count_;
}

bool DebugArgs::Iterator::operator!=(const Iterator &other) const {
  return !(*this == other);
}

// ---
// `DebugArgs` Implementation
// ---

DebugArgs::DebugArgs() : address_(nullptr), storage_size_(0), count_(0) {}

DebugArgs::DebugArgs(const void *const address, const size_t storage_size,
                     const size_t count)
    : address_(address), storage_size_(storage_size), count_(count) {}

size_t DebugArgs::size() const { return count_; }

DebugArgs::Iterator DebugArgs::begin() const {
  return Iterator(address_, count_);
}

DebugArgs::Iterator DebugArgs::end() const {
  return Iterator(static_cast<const void *>(
                      static_cast<const uint8_t *>(address_) + storage_size_),
                  0);
}

}  // namespace inspector