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

#include <inspector/details/trace_event.hpp>

#include <cstring>
#include <string>

#define PACKED __attribute__((packed))

namespace inspector {
namespace details {
namespace TraceEvent {
namespace {

/**
 * @brief Type trait to get `DebugArgType` enum value for an object type `T`.
 *
 * @tparam T Type of object.
 */
template <class T>
struct TypeId {};

// Template specializations for the different supported data types.

template <>
struct TypeId<int16_t> {
  static constexpr auto value = DebugArgType::TYPE_INT16;
};

template <>
struct TypeId<int32_t> {
  static constexpr auto value = DebugArgType::TYPE_INT32;
};

template <>
struct TypeId<int64_t> {
  static constexpr auto value = DebugArgType::TYPE_INT64;
};

template <>
struct TypeId<uint8_t> {
  static constexpr auto value = DebugArgType::TYPE_UINT8;
};

template <>
struct TypeId<uint16_t> {
  static constexpr auto value = DebugArgType::TYPE_UINT16;
};

template <>
struct TypeId<uint32_t> {
  static constexpr auto value = DebugArgType::TYPE_UINT32;
};

template <>
struct TypeId<uint64_t> {
  static constexpr auto value = DebugArgType::TYPE_UINT64;
};

template <>
struct TypeId<float> {
  static constexpr auto value = DebugArgType::TYPE_FLOAT;
};

template <>
struct TypeId<double> {
  static constexpr auto value = DebugArgType::TYPE_DOUBLE;
};

template <>
struct TypeId<char> {
  static constexpr auto value = DebugArgType::TYPE_CHAR;
};

template <>
struct TypeId<const char*> {
  static constexpr auto value = DebugArgType::TYPE_STRING;
};

/**
 * @brief The data structure `Header` contains all the header information
 * of a single captured trace or metric event using the inspector library.
 *
 */
struct PACKED Header {
  event_type_t type;  //<- Type of event. This field is used to distinguish the
                      // different events.
  event_category_t category;  //<- Category of event.
  uint64_t counter;           //<- Counter used to track for lost events.
  timestamp_t timestamp;      //<- Timestamp in nano seconds when the
                              // event occurred.
  int32_t pid;  //<- Identifier of the process in which the event occurred or
                // is observed.
  int32_t tid;  //<- Identifier of the thread in which the event occurred or
                // is observed.
  uint8_t args_count;  //<- Number of debug arguments stored in the event.
};

/**
 * @brief Make `DebugArgs` from pointer to the trace event header.
 *
 * @param header Constant pointer to trace event header.
 * @returns `DebugArgs` object.
 */
DebugArgs makeDebugArgs(const void* const header) {
  return DebugArgs(static_cast<const void*>(
                       static_cast<const uint8_t*>(header) + sizeof(Header)),
                   static_cast<const Header*>(header)->args_count);
}

}  // namespace

// ---
// `DebugArgs::Iterator` Implementation
// ---

DebugArgs::Iterator::Iterator()
    : debug_args_(nullptr), current_(nullptr), counts_(0) {}

DebugArgs::Iterator::Iterator(const DebugArgs& debug_args,
                              const void* const address, const size_t counts)
    : debug_args_(std::addressof(debug_args)),
      current_(address),
      counts_(counts) {}

DebugArgType DebugArgs::Iterator::type() const {
  return static_cast<const DebugArgType>(
      *static_cast<const uint8_t*>(current_));
}

template <class T>
T DebugArgs::Iterator::get() const {
  if (type() != TypeId<T>::value) {
    throw std::runtime_error("Invalid type specified for argument of type '" +
                             std::to_string(static_cast<int>(type())) + "'.");
  }
  return *static_cast<const T*>(static_cast<const void*>(
      static_cast<const uint8_t*>(current_) + sizeof(uint8_t)));
}

// Instantiating for different supported data types
template int16_t DebugArgs::Iterator::get<int16_t>() const;
template int32_t DebugArgs::Iterator::get<int32_t>() const;
template int64_t DebugArgs::Iterator::get<int64_t>() const;
template uint8_t DebugArgs::Iterator::get<uint8_t>() const;
template uint16_t DebugArgs::Iterator::get<uint16_t>() const;
template uint32_t DebugArgs::Iterator::get<uint32_t>() const;
template uint64_t DebugArgs::Iterator::get<uint64_t>() const;
template float DebugArgs::Iterator::get<float>() const;
template double DebugArgs::Iterator::get<double>() const;
template char DebugArgs::Iterator::get<char>() const;

// Template specialization for string literals
template <>
const char* DebugArgs::Iterator::get<const char*>() const {
  if (type() != TypeId<const char*>::value) {
    throw std::runtime_error("Invalid type specified for argument of type '" +
                             std::to_string(static_cast<int>(type())) + "'.");
  }
  return static_cast<const char*>(static_cast<const void*>(
      static_cast<const uint8_t*>(current_) + sizeof(uint8_t)));
}

// Template specialization for std::string
template <>
std::string DebugArgs::Iterator::get<std::string>() const {
  return std::string{get<const char*>()};
}

DebugArgs::Iterator& DebugArgs::Iterator::operator++() {
  if (counts_ && current_) {
    size_t size = sizeof(uint8_t);
    switch (type()) {
      case DebugArgType::TYPE_INT16: {
        size += sizeof(int16_t);
        break;
      }
      case DebugArgType::TYPE_INT32: {
        size += sizeof(int32_t);
        break;
      }
      case DebugArgType::TYPE_INT64: {
        size += sizeof(int64_t);
        break;
      }
      case DebugArgType::TYPE_UINT8: {
        size += sizeof(uint8_t);
        break;
      }
      case DebugArgType::TYPE_UINT16: {
        size += sizeof(uint16_t);
        break;
      }
      case DebugArgType::TYPE_UINT32: {
        size += sizeof(uint32_t);
        break;
      }
      case DebugArgType::TYPE_UINT64: {
        size += sizeof(uint64_t);
        break;
      }
      case DebugArgType::TYPE_FLOAT: {
        size += sizeof(float);
        break;
      }
      case DebugArgType::TYPE_DOUBLE: {
        size += sizeof(double);
        break;
      }
      case DebugArgType::TYPE_CHAR: {
        size += sizeof(char);
        break;
      }
      case DebugArgType::TYPE_STRING: {
        size += std::strlen(static_cast<const char*>(current_)) * sizeof(char) +
                sizeof(char);
        break;
      }
    }
    current_ =
        static_cast<const void*>(static_cast<const uint8_t*>(current_) + size);
    --counts_;
  }
  return *this;
}

bool DebugArgs::Iterator::operator==(const DebugArgs::Iterator& other) const {
  return debug_args_ == other.debug_args_ && counts_ == other.counts_;
}

bool DebugArgs::Iterator::operator!=(const DebugArgs::Iterator& other) const {
  return !(*this == other);
}

// ---
// `DebugArgs` Implementation
// ---

// static
template <class T>
size_t DebugArgs::storageSize(const T& arg) {
  // The first 8 bits store the type of data while the rest store the actual
  // data.
  return sizeof(uint8_t) + sizeof(arg);
}

// Instantiating for different supported data types
template size_t DebugArgs::storageSize<int16_t>(const int16_t&);
template size_t DebugArgs::storageSize<int32_t>(const int32_t&);
template size_t DebugArgs::storageSize<int64_t>(const int64_t&);
template size_t DebugArgs::storageSize<uint8_t>(const uint8_t&);
template size_t DebugArgs::storageSize<uint16_t>(const uint16_t&);
template size_t DebugArgs::storageSize<uint32_t>(const uint32_t&);
template size_t DebugArgs::storageSize<uint64_t>(const uint64_t&);
template size_t DebugArgs::storageSize<float>(const float&);
template size_t DebugArgs::storageSize<double>(const double&);
template size_t DebugArgs::storageSize<char>(const char&);

// Template specialization for string literal
template <>
size_t DebugArgs::storageSize<const char*>(const char* const& obj) {
  // The size includes first 8 bits for type, followed by size for storing the
  // chars in the string including the null terminator.
  return sizeof(uint8_t) + std::strlen(obj) * sizeof(char) + sizeof(char);
}

// Template specialization for std::string
template <>
size_t DebugArgs::storageSize<std::string>(const std::string& obj) {
  return DebugArgs::storageSize(obj.c_str());
}

DebugArgs::DebugArgs(const void* const address, const size_t counts)
    : address_(address), counts_(counts) {}

size_t DebugArgs::size() const { return counts_; }

DebugArgs::Iterator DebugArgs::begin() const {
  return Iterator(*this, address_, counts_);
}

DebugArgs::Iterator DebugArgs::end() const {
  return Iterator(*this, address_, 0);
}

// ---
// `storageSize` Implementation
// ---

size_t storageSize() { return sizeof(Header); }

// ---
// `View` Implementation
// ---

View::View(const void* const address) : address_(address) {}

event_type_t View::type() const {
  return static_cast<const TraceEvent::Header*>(address_)->type;
}

event_category_t View::category() const {
  return static_cast<const TraceEvent::Header*>(address_)->category;
}

uint64_t View::counter() const {
  return static_cast<const TraceEvent::Header*>(address_)->counter;
}

timestamp_t View::timestampNs() const {
  return static_cast<const TraceEvent::Header*>(address_)->timestamp;
}

int32_t View::pid() const {
  return static_cast<const TraceEvent::Header*>(address_)->pid;
}

int32_t View::tid() const {
  return static_cast<const TraceEvent::Header*>(address_)->tid;
}

const char* View::name() const {
  auto debug_args = makeDebugArgs(address_);
  auto it = debug_args.begin();
  if (it == debug_args.end()) {
    return nullptr;
  }
  return it.get<const char*>();
}

DebugArgs View::debugArgs() const {
  auto debug_args = makeDebugArgs(address_);
  auto it = debug_args.begin();
  if (it == debug_args.end()) {
    return DebugArgs{nullptr, 0};
  }
  ++it;
  return DebugArgs(it.current_, it.counts_);
}

// ---
// `MutableView` Implementation
// ---

MutableView::MutableView(void* const address) : address_(address) {}

void MutableView::setType(const event_type_t type) {
  static_cast<TraceEvent::Header*>(address_)->type = type;
}

event_type_t MutableView::type() const {
  return static_cast<TraceEvent::Header*>(address_)->type;
}

void MutableView::setCategory(const event_category_t category) {
  static_cast<TraceEvent::Header*>(address_)->category = category;
}

event_category_t MutableView::category() const {
  return static_cast<TraceEvent::Header*>(address_)->category;
}

void MutableView::setCounter(const uint64_t counter) {
  static_cast<TraceEvent::Header*>(address_)->counter = counter;
}

uint64_t MutableView::counter() const {
  return static_cast<TraceEvent::Header*>(address_)->counter;
}

void MutableView::setTimestampNs(const timestamp_t timestamp_ns) {
  static_cast<TraceEvent::Header*>(address_)->timestamp = timestamp_ns;
}

timestamp_t MutableView::timestampNs() const {
  return static_cast<TraceEvent::Header*>(address_)->timestamp;
}

void MutableView::setPid(const int32_t pid) {
  static_cast<TraceEvent::Header*>(address_)->pid = pid;
}

int32_t MutableView::pid() const {
  return static_cast<TraceEvent::Header*>(address_)->pid;
}

void MutableView::setTid(const int32_t tid) {
  static_cast<TraceEvent::Header*>(address_)->tid = tid;
}

int32_t MutableView::tid() const {
  return static_cast<TraceEvent::Header*>(address_)->tid;
}

const char* MutableView::name() const {
  auto debug_args = makeDebugArgs(address_);
  auto it = debug_args.begin();
  if (it == debug_args.end()) {
    return nullptr;
  }
  return it.get<const char*>();
}

uint8_t MutableView::debugArgsCount() const {
  return static_cast<TraceEvent::Header*>(address_)->args_count;
}

template <class T>
size_t MutableView::addDebugArgAt(const size_t offset, const T& arg) {
  void* const args_end = static_cast<void*>(
      static_cast<uint8_t*>(address_) + sizeof(TraceEvent::Header) + offset);
  *static_cast<uint8_t*>(args_end) = static_cast<uint8_t>(TypeId<T>::value);
  std::memcpy(
      static_cast<void*>(static_cast<uint8_t*>(args_end) + sizeof(uint8_t)),
      static_cast<const void*>(&arg), sizeof(arg));
  static_cast<TraceEvent::Header*>(address_)->args_count += 1;

  return DebugArgs::storageSize(arg);
}

// Instantiating for different supported data types
template size_t MutableView::addDebugArgAt<int16_t>(const size_t,
                                                    const int16_t&);
template size_t MutableView::addDebugArgAt<int32_t>(const size_t,
                                                    const int32_t&);
template size_t MutableView::addDebugArgAt<int64_t>(const size_t,
                                                    const int64_t&);
template size_t MutableView::addDebugArgAt<uint8_t>(const size_t,
                                                    const uint8_t&);
template size_t MutableView::addDebugArgAt<uint16_t>(const size_t,
                                                     const uint16_t&);
template size_t MutableView::addDebugArgAt<uint32_t>(const size_t,
                                                     const uint32_t&);
template size_t MutableView::addDebugArgAt<uint64_t>(const size_t,
                                                     const uint64_t&);
template size_t MutableView::addDebugArgAt<float>(const size_t, const float&);
template size_t MutableView::addDebugArgAt<double>(const size_t, const double&);
template size_t MutableView::addDebugArgAt<char>(const size_t, const char&);

// Template specialization for string literal
template <>
size_t MutableView::addDebugArgAt<const char*>(const size_t offset,
                                               const char* const& arg) {
  void* const args_end = static_cast<void*>(
      static_cast<uint8_t*>(address_) + sizeof(TraceEvent::Header) + offset);
  *static_cast<uint8_t*>(args_end) =
      static_cast<uint8_t>(TypeId<const char*>::value);
  const size_t str_size = sizeof(char) * std::strlen(arg) + sizeof(char);
  std::memcpy(
      static_cast<void*>(static_cast<uint8_t*>(args_end) + sizeof(uint8_t)),
      static_cast<const void*>(arg), str_size);
  static_cast<TraceEvent::Header*>(address_)->args_count += 1;

  return str_size + sizeof(uint8_t);
}

// Template specialization for string
template <>
size_t MutableView::addDebugArgAt<std::string>(const size_t offset,
                                               const std::string& arg) {
  return addDebugArgAt(offset, arg.c_str());
}

}  // namespace TraceEvent
}  // namespace details
}  // namespace inspector