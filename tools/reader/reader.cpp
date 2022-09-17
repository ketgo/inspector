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

#include "tools/reader/reader.hpp"

#include <inspector/details/system.hpp>

#include "tools/reader/basic_reader.hpp"

namespace inspector {

// ---------------------------------
// Reader::Iterator Implementation
// ---------------------------------

Reader::Iterator::Iterator(Buffer& buffer,
                           const std::chrono::microseconds& timeout_ms)
    : event_(0, {}),
      buffer_(std::addressof(buffer)),
      timeout_ms_(timeout_ms),
      timeout_(false) {}

Reader::Iterator::Iterator(Buffer& buffer, bool timeout)
    : event_(0, {}),
      buffer_(std::addressof(buffer)),
      timeout_ms_(0),
      timeout_(timeout) {}

void Reader::Iterator::Next() {
  if (timeout_ms_.count()) {
    timeout_ = !(buffer_->Pop(event_, timeout_ms_));
  } else {
    buffer_->Pop(event_);
  }
}

// ---------- public --------------

Reader::Iterator::Iterator()
    : event_(0, {}), buffer_(nullptr), timeout_ms_(0), timeout_(false) {}

Event* Reader::Iterator::operator->() { return &event_.second; }

Event& Reader::Iterator::operator*() { return event_.second; }

Reader::Iterator& Reader::Iterator::operator++() {
  Next();
  return *this;
}

Reader::Iterator Reader::Iterator::operator++(int) {
  Iterator rvalue = *this;
  Next();
  return rvalue;
}

bool Reader::Iterator::operator==(const Iterator& other) const {
  return buffer_ == other.buffer_ && timeout_ == other.timeout_;
}

bool Reader::Iterator::operator!=(const Iterator& other) const {
  return !(*this == other);
}

// ---------------------------------
// Reader Implementation
// ---------------------------------

void Reader::StartWorkers() {
  for (auto& worker : workers_) {
    worker = std::thread([&]() {
      while (!stop_.load()) {
        BasicReader reader(*queue_, max_read_attempt_);
        for (auto&& event : reader) {
          // Push the event to the priority queue buffer
          buffer_.Push({event.Timestamp(), std::move(event)});
        }
        std::this_thread::sleep_for(polling_interval_ms_);
      }
    });
  }
}

void Reader::StopWorkers() {
  stop_.store(true);
  for (auto& worker : workers_) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

// ----------- public --------------

Reader::Reader(const std::string& queue_name,
               const std::size_t max_read_attempt,
               const std::chrono::microseconds& polling_interval_ms,
               const std::size_t worker_count,

               const int64_t buffer_window_size)
    : queue_(details::system::GetSharedObject<details::EventQueue>(queue_name)),
      max_read_attempt_(max_read_attempt),
      polling_interval_ms_(polling_interval_ms),
      workers_(worker_count),
      buffer_(buffer_window_size),
      stop_(false),
      timeout_ms_(0) {
  StartWorkers();
}

Reader::Reader(const std::chrono::microseconds& timeout_ms,
               const std::string& queue_name,
               const std::size_t max_read_attempt,
               const std::chrono::microseconds& polling_interval_ms,
               const std::size_t worker_count, const int64_t buffer_window_size)
    : queue_(details::system::GetSharedObject<details::EventQueue>(queue_name)),
      max_read_attempt_(max_read_attempt),
      polling_interval_ms_(polling_interval_ms),
      workers_(worker_count),
      buffer_(buffer_window_size),
      stop_(false),
      timeout_ms_(timeout_ms) {
  StartWorkers();
}

Reader::~Reader() { StopWorkers(); }

Reader::Iterator Reader::begin() const {
  Iterator it(buffer_, timeout_ms_);
  return ++it;
}

Reader::Iterator Reader::end() const { return {buffer_, true}; }

}  // namespace inspector