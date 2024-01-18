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
#include <sstream>

#include <inspector/trace.hpp>

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

/**
 * @brief Method to get string representation of an event type.
 *
 * @param type Type of event.
 * @returns String representation of event type.
 */
std::string eventTypeToString(event_type_t type) {
  switch (static_cast<EventType>(type)) {
    case EventType::kSyncBeginTag:
      return "SyncBegin";
    case EventType::kSyncEndTag:
      return "AyncEnd";
    case EventType::kAsyncBeginTag:
      return "AsyncBegin";
    case EventType::kAsyncInstanceTag:
      return "AsyncInstance";
    case EventType::kAsyncEndTag:
      return "AsyncEnd";
    case EventType::kFlowBeginTag:
      return "FlowBegin";
    case EventType::kFlowInstanceTag:
      return "FlowInstance";
    case EventType::kFlowEndTag:
      return "FlowEnd";
    case EventType::kCounterTag:
      return "Counter";
    default:
      break;
  }

  return "UNKNOWN";
}

/**
 * @brief Method to get string representation of a debug argument.
 *
 * @param arg Constant reference to a debug argument.
 * @returns String representation of debug argument.
 */
std::string debugArgToString(const DebugArg &arg) {
  switch (arg.type()) {
    case DebugArg::Type::TYPE_STRING:
    case DebugArg::Type::TYPE_CHAR:
      return "\"" + arg.value<std::string>() + "\"";
    case DebugArg::Type::TYPE_INT16:
      return std::to_string(arg.value<int16_t>());
    case DebugArg::Type::TYPE_INT32:
      return std::to_string(arg.value<int32_t>());
    case DebugArg::Type::TYPE_INT64:
      return std::to_string(arg.value<int64_t>());
    case DebugArg::Type::TYPE_UINT8:
      return std::to_string(arg.value<uint8_t>());
    case DebugArg::Type::TYPE_UINT16:
      return std::to_string(arg.value<uint16_t>());
    case DebugArg::Type::TYPE_UINT32:
      return std::to_string(arg.value<uint32_t>());
    case DebugArg::Type::TYPE_UINT64:
      return std::to_string(arg.value<uint64_t>());
    case DebugArg::Type::TYPE_FLOAT:
      return std::to_string(arg.value<float>());
    case DebugArg::Type::TYPE_DOUBLE:
      return std::to_string(arg.value<double>());
    default:
      break;
  }

  return "UNKNOWN";
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

std::string TraceEvent::toJson() const {
  std::stringstream out;

  out << "{\"seq_num\":" << counter() << ",\"timestamp\":" << timestampNs()
      << ",\"pid\":" << pid() << ",\"tid\":" << tid() << ",\"type\":\""
      << eventTypeToString(type()) << "\",\"name\":\"" << name() << "\"";
  if (debugArgs().size()) {
    out << ",\"args\":[";
    auto debug_args = debugArgs();
    auto it = debug_args.begin();
    out << debugArgToString(*it);
    while (++it != debug_args.end()) {
      out << "," << debugArgToString(*it);
    }
    out << "]";
  }
  out << "}";

  return out.str();
}

}  // namespace inspector

#undef THROW_IF_EMPTY