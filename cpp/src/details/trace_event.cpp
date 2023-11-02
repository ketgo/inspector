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

#include "cpp/src/details/trace_event_header.hpp"
#include "cpp/src/details/type_traits.hpp"

namespace inspector {
namespace details {

// ---
// `storageSize` Implementation
// ---

size_t traceEventStorageSize() { return sizeof(TraceEventHeader); }

// ---
// `MutableTraceEvent` Implementation
// ---

MutableTraceEvent::MutableTraceEvent(void *const address, const size_t size)
    : address_(address),
      size_(size),
      debug_args_head_(static_cast<void *>(static_cast<uint8_t *>(address_) +
                                           sizeof(TraceEventHeader))) {}

void MutableTraceEvent::setType(const event_type_t type) {
  static_cast<TraceEventHeader *>(address_)->type = type;
}

void MutableTraceEvent::setCounter(const uint64_t counter) {
  static_cast<TraceEventHeader *>(address_)->counter = counter;
}

void MutableTraceEvent::setTimestampNs(const timestamp_t timestamp_ns) {
  static_cast<TraceEventHeader *>(address_)->timestamp = timestamp_ns;
}

void MutableTraceEvent::setPid(const int32_t pid) {
  static_cast<TraceEventHeader *>(address_)->pid = pid;
}

void MutableTraceEvent::setTid(const int32_t tid) {
  static_cast<TraceEventHeader *>(address_)->tid = tid;
}

template <class T>
void MutableTraceEvent::appendDebugArg(const T &arg) {
  // Skip appending argument if the buffer is full.
  if (static_cast<void *>(static_cast<uint8_t *>(address_) + size_) <=
      debug_args_head_) {
    return;
  }

  *static_cast<uint8_t *>(debug_args_head_) =
      static_cast<uint8_t>(TypeId<T>::value);
  std::memcpy(static_cast<void *>(static_cast<uint8_t *>(debug_args_head_) + 1),
              static_cast<const void *>(&arg), sizeof(arg));
  static_cast<TraceEventHeader *>(address_)->args_count += 1;
  debug_args_head_ = static_cast<void *>(
      static_cast<uint8_t *>(debug_args_head_) + debugArgsStorageSize(arg));
}

// Instantiating for different supported data types
template void MutableTraceEvent::appendDebugArg<int16_t>(const int16_t &);
template void MutableTraceEvent::appendDebugArg<int32_t>(const int32_t &);
template void MutableTraceEvent::appendDebugArg<int64_t>(const int64_t &);
template void MutableTraceEvent::appendDebugArg<uint8_t>(const uint8_t &);
template void MutableTraceEvent::appendDebugArg<uint16_t>(const uint16_t &);
template void MutableTraceEvent::appendDebugArg<uint32_t>(const uint32_t &);
template void MutableTraceEvent::appendDebugArg<uint64_t>(const uint64_t &);
template void MutableTraceEvent::appendDebugArg<float>(const float &);
template void MutableTraceEvent::appendDebugArg<double>(const double &);
template void MutableTraceEvent::appendDebugArg<char>(const char &);

// Template specialization for c-string
template <>
void MutableTraceEvent::appendDebugArg<const char *>(const char *const &arg) {
  // Skip appending argument if the buffer is full.
  if (static_cast<void *>(static_cast<uint8_t *>(address_) + size_) <=
      debug_args_head_) {
    return;
  }

  *static_cast<uint8_t *>(debug_args_head_) =
      static_cast<uint8_t>(TypeId<const char *>::value);
  const size_t str_size = std::strlen(arg) * sizeof(char) + sizeof(char);
  std::memcpy(static_cast<void *>(static_cast<uint8_t *>(debug_args_head_) + 1),
              static_cast<const void *>(arg), str_size);
  static_cast<TraceEventHeader *>(address_)->args_count += 1;
  debug_args_head_ = static_cast<void *>(
      static_cast<uint8_t *>(debug_args_head_) + str_size + sizeof(uint8_t));
}

// Template specialization for string
template <>
void MutableTraceEvent::appendDebugArg<std::string>(const std::string &arg) {
  return appendDebugArg(arg.c_str());
}

}  // namespace details
}  // namespace inspector