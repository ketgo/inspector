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

#pragma once

#include <cstring>

#include <inspector/details/event_queue.hpp>

namespace inspector {

// Forward declared reader class so that it can be friended
class Reader;

namespace details {

/**
 * @brief Iterator to iterate over events in the event queue.
 *
 * The Iterator class is a forward-only direction iterator. It consumes events
 * from the event queue while iterating.
 *
 */
class Iterator {
  // Reader needs to access the private CTOR
  friend class ::inspector::Reader;

 public:
  Iterator();
  std::string* operator->();
  std::string& operator*();
  Iterator& operator++();
  Iterator operator++(int);
  bool operator==(const Iterator& other) const;
  bool operator!=(const Iterator& other) const;

 private:
  Iterator(const EventQueue* queue, const std::size_t max_attempt,
           EventQueue::Status status);

  void Next();

  EventQueue* queue_;
  std::size_t max_attempt_;
  EventQueue::Status status_;
  std::string event_;
};

// ---------------------------------
// Iterator Implementation
// ---------------------------------

Iterator::Iterator(const EventQueue* queue, const std::size_t max_attempt,
                   EventQueue::Status status)
    : queue_(const_cast<EventQueue*>(queue)),
      max_attempt_(max_attempt),
      status_(status) {}

void Iterator::Next() {
  EventQueue::ReadSpan span;
  status_ = queue_->Consume(span, max_attempt_);
  if (status_ == EventQueue::Status::OK) {
    event_ = {span.Data(), span.Size()};
  }
}

// ------------ public -------------

Iterator::Iterator()
    : queue_(nullptr), max_attempt_(0), status_(EventQueue::Status::OK) {}

std::string* Iterator::operator->() { return &event_; }

std::string& Iterator::operator*() { return event_; }

Iterator& Iterator::operator++() {
  Next();
  return *this;
}

Iterator Iterator::operator++(int) {
  Iterator rvalue = *this;
  Next();
  return rvalue;
}

bool Iterator::operator==(const Iterator& other) const {
  return queue_ == other.queue_ && status_ == other.status_;
}

bool Iterator::operator!=(const Iterator& other) const {
  return !(*this == other);
}

// ---------------------------------

}  // namespace details
}  // namespace inspector