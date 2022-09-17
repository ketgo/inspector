/**
 * Copyright 2022 Ketan Goyal
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

#include "tools/reader/event.hpp"

#include <cassert>

#include <inspector/details/event_header.hpp>

namespace inspector {

// ------------------------------------------------------
// Event Implementation
// ------------------------------------------------------

// static
Event Event::Parse(const char* start, const std::size_t size) {
  Event event;

  assert(size > sizeof(details::EventHeader));
  auto header = reinterpret_cast<const details::EventHeader*>(start);
  event.type_ = header->type;
  event.timestamp_ = header->timestamp;
  event.pid_ = header->pid;
  event.tid_ = header->tid;
  event.payload_ = std::string(start + sizeof(details::EventHeader),
                               size - sizeof(details::EventHeader));

  return event;
}

const int8_t& Event::Type() const { return type_; }

const int64_t& Event::Timestamp() const { return timestamp_; }

const int32_t& Event::Pid() const { return pid_; }

const int32_t& Event::Tid() const { return tid_; }

const std::string& Event::Payload() const { return payload_; }

bool Event::operator==(const Event& other) const {
  return type_ == other.type_ && timestamp_ == other.timestamp_ &&
         pid_ == other.pid_ && tid_ == other.tid_ && payload_ == other.payload_;
}

bool Event::operator!=(const Event& other) const { return !(*this == other); }

// ------------------------------------------------------

}  // namespace inspector