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

MutableTraceEvent::MutableTraceEvent(void* const address) : address_(address) {}

void MutableTraceEvent::setType(const event_type_t type) {
  static_cast<TraceEventHeader*>(address_)->type = type;
}

void MutableTraceEvent::setCounter(const uint64_t counter) {
  static_cast<TraceEventHeader*>(address_)->counter = counter;
}

void MutableTraceEvent::setTimestampNs(const timestamp_t timestamp_ns) {
  static_cast<TraceEventHeader*>(address_)->timestamp = timestamp_ns;
}

void MutableTraceEvent::setPid(const int32_t pid) {
  static_cast<TraceEventHeader*>(address_)->pid = pid;
}

void MutableTraceEvent::setTid(const int32_t tid) {
  static_cast<TraceEventHeader*>(address_)->tid = tid;
}

template <class T>
size_t MutableTraceEvent::addDebugArgAt(const size_t offset, const T& arg) {
  void* const args_end = static_cast<void*>(static_cast<uint8_t*>(address_) +
                                            sizeof(TraceEventHeader) + offset);
  *static_cast<uint8_t*>(args_end) = static_cast<uint8_t>(TypeId<T>::value);
  std::memcpy(
      static_cast<void*>(static_cast<uint8_t*>(args_end) + sizeof(uint8_t)),
      static_cast<const void*>(&arg), sizeof(arg));
  static_cast<TraceEventHeader*>(address_)->args_count += 1;

  return debugArgStorageSize(arg);
}

// Instantiating for different supported data types
template size_t MutableTraceEvent::addDebugArgAt<int16_t>(const size_t,
                                                          const int16_t&);
template size_t MutableTraceEvent::addDebugArgAt<int32_t>(const size_t,
                                                          const int32_t&);
template size_t MutableTraceEvent::addDebugArgAt<int64_t>(const size_t,
                                                          const int64_t&);
template size_t MutableTraceEvent::addDebugArgAt<uint8_t>(const size_t,
                                                          const uint8_t&);
template size_t MutableTraceEvent::addDebugArgAt<uint16_t>(const size_t,
                                                           const uint16_t&);
template size_t MutableTraceEvent::addDebugArgAt<uint32_t>(const size_t,
                                                           const uint32_t&);
template size_t MutableTraceEvent::addDebugArgAt<uint64_t>(const size_t,
                                                           const uint64_t&);
template size_t MutableTraceEvent::addDebugArgAt<float>(const size_t,
                                                        const float&);
template size_t MutableTraceEvent::addDebugArgAt<double>(const size_t,
                                                         const double&);
template size_t MutableTraceEvent::addDebugArgAt<char>(const size_t,
                                                       const char&);

// Template specialization for string literal
template <>
size_t MutableTraceEvent::addDebugArgAt<const char*>(const size_t offset,
                                                     const char* const& arg) {
  void* const args_end = static_cast<void*>(static_cast<uint8_t*>(address_) +
                                            sizeof(TraceEventHeader) + offset);
  *static_cast<uint8_t*>(args_end) =
      static_cast<uint8_t>(TypeId<const char*>::value);
  const size_t str_size = sizeof(char) * std::strlen(arg) + sizeof(char);
  std::memcpy(
      static_cast<void*>(static_cast<uint8_t*>(args_end) + sizeof(uint8_t)),
      static_cast<const void*>(arg), str_size);
  static_cast<TraceEventHeader*>(address_)->args_count += 1;

  return str_size + sizeof(uint8_t);
}

// Template specialization for string
template <>
size_t MutableTraceEvent::addDebugArgAt<std::string>(const size_t offset,
                                                     const std::string& arg) {
  return addDebugArgAt(offset, arg.c_str());
}

}  // namespace details
}  // namespace inspector