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

#include <inspector/trace_event.hpp>

#include <stdexcept>

#include "cpp/src/details/trace_event_header.hpp"

#define THROW_IF_EMPTY(buffer) \
  if (buffer.empty()) throw std::runtime_error("Empty trace event.");

namespace inspector {
namespace {

/**
 * @brief Method to get pointer to trace event header from a memory buffer
 * containing the trace event.
 *
 * @param buffer Constant reference to the buffer.
 * @returns Pointer to constant trace event header.
 */
const details::TraceEventHeader *header(const std::vector<uint8_t> &buffer) {
  return static_cast<const details::TraceEventHeader *>(
      static_cast<const void *>(buffer.data()));
}

}  // namespace

TraceEvent::TraceEvent(std::vector<uint8_t> &&buffer)
    : buffer_(std::move(buffer)) {}

bool TraceEvent::isEmpty() const { return buffer_.empty(); }

event_type_t TraceEvent::type() const {
  THROW_IF_EMPTY(buffer_);
  return header(buffer_)->type;
}

uint64_t TraceEvent::counter() const {
  THROW_IF_EMPTY(buffer_);
  return header(buffer_)->counter;
}

timestamp_t TraceEvent::timestampNs() const {
  THROW_IF_EMPTY(buffer_);
  return header(buffer_)->timestamp;
}

int32_t TraceEvent::pid() const {
  THROW_IF_EMPTY(buffer_);
  return header(buffer_)->pid;
}

int32_t TraceEvent::tid() const {
  THROW_IF_EMPTY(buffer_);
  return header(buffer_)->tid;
}

const char *TraceEvent::name() const {
  THROW_IF_EMPTY(buffer_);
  const void *address = static_cast<const void *>(
      buffer_.data() + sizeof(details::TraceEventHeader));
  const size_t storage_size =
      buffer_.size() - sizeof(details::TraceEventHeader);
  auto debug_args =
      DebugArgs(address, storage_size, header(buffer_)->args_count);
  auto it = debug_args.begin();
  if (it == debug_args.end()) {
    return nullptr;
  }
  return it->value<const char *>();
}

DebugArgs TraceEvent::debugArgs() const {
  THROW_IF_EMPTY(buffer_);
  const void *address = static_cast<const void *>(
      buffer_.data() + sizeof(details::TraceEventHeader));
  const size_t storage_size =
      buffer_.size() - sizeof(details::TraceEventHeader);
  auto debug_args =
      DebugArgs(address, storage_size, header(buffer_)->args_count);
  auto it = debug_args.begin();
  if (it == debug_args.end()) {
    return DebugArgs{};
  }
  ++it;
  const size_t new_storage_size =
      buffer_.size() -
      static_cast<size_t>(static_cast<const uint8_t *>(it->address()) -
                          buffer_.data());
  return DebugArgs(it->address(), new_storage_size,
                   header(buffer_)->args_count - 1);
}

}  // namespace inspector

#undef THROW_IF_EMPTY