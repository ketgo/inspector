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

#include <iostream>

#include <inspector/trace_reader.hpp>

// TODO: Signal handler

namespace inspector {

// ---------------------------------
// Reader::Iterator Implementation
// ---------------------------------

Reader::Iterator::Iterator() : value_(), queue_(nullptr), closed_(true) {}

Reader::Iterator::Iterator(event_queue_t& queue, const bool closed)
    : value_(), queue_(std::addressof(queue)), closed_(closed) {
  next();
}

void Reader::Iterator::next() {
  if (closed_ || queue_ == nullptr) {
    return;
  }

  auto result = queue_->pop();
  if (result.first == event_queue_t::Result::kClosed) {
    closed_ = true;
    return;
  }
  value_ = std::move(result.second);
}

TraceEvent* Reader::Iterator::operator->() { return &value_.second; }

TraceEvent& Reader::Iterator::operator*() { return value_.second; }

Reader::Iterator& Reader::Iterator::operator++() {
  next();
  return *this;
}

Reader::Iterator Reader::Iterator::operator++(int) {
  Iterator rvalue = *this;
  next();
  return rvalue;
}

bool Reader::Iterator::operator==(const Iterator& other) const {
  return queue_ == other.queue_ && closed_ == other.closed_;
}

bool Reader::Iterator::operator!=(const Iterator& other) const {
  return !(*this == other);
}

// ---------------------------------
// Reader Implementation
// ---------------------------------

Reader::Reader(const std::chrono::microseconds timeout_us,
               const std::chrono::microseconds polling_interval_us,
               const size_t num_consumers, const duration_t min_window_size,
               const duration_t max_window_size)
    : timeout_us_(timeout_us),
      polling_interval_us_(polling_interval_us),
      consumers_(num_consumers),
      queue_(min_window_size, max_window_size),
      count_(0),
      stop_(false) {
  for (auto& consumer : consumers_) {
    consumer = std::thread([&]() {
      auto pause_duration = std::chrono::microseconds{0};
      while (!stop_.load() && pause_duration <= timeout_us_) {
        bool pause = false;
        while (!pause) {
          auto event = readTraceEvent();
          if (event.isEmpty()) {
            pause = true;
          } else {
            queue_.push({event.timestampNs(), std::move(event)});
          }
        }
        std::this_thread::sleep_for(polling_interval_us_);
        pause_duration += polling_interval_us_;
      }
      if (++count_ == consumers_.size()) {
        queue_.close();
      }
    });
  }
}

Reader::~Reader() {
  stop_.store(true);
  for (auto& consumer : consumers_) {
    if (consumer.joinable()) {
      consumer.join();
    }
  }
}

Reader::Iterator Reader::begin() const { return Iterator(queue_, false); }

Reader::Iterator Reader::end() const { return Iterator(queue_, true); }

}  // namespace inspector