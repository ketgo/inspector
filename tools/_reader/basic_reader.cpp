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

#include "tools/reader/basic_reader.hpp"

#include <inspector/details/system.hpp>

namespace inspector {

// ---------------------------------
// BasicReader::Iterator Implementation
// ---------------------------------

BasicReader::Iterator::Iterator(const details::EventQueue* queue,
                                const std::size_t max_attempt,
                                details::EventQueue::Status status)
    : queue_(const_cast<details::EventQueue*>(queue)),
      max_attempt_(max_attempt),
      status_(status) {}

void BasicReader::Iterator::Next() {
  details::EventQueue::ReadSpan span;
  status_ = queue_->Consume(span, max_attempt_);
  if (status_ == details::EventQueue::Status::OK) {
    event_ = Event::Parse(span.Data(), span.Size());
  }
}

// ------------ public -------------

BasicReader::Iterator::Iterator()
    : queue_(nullptr),
      max_attempt_(0),
      status_(details::EventQueue::Status::OK) {}

Event* BasicReader::Iterator::operator->() { return &event_; }

Event& BasicReader::Iterator::operator*() { return event_; }

BasicReader::Iterator& BasicReader::Iterator::operator++() {
  Next();
  return *this;
}

BasicReader::Iterator BasicReader::Iterator::operator++(int) {
  Iterator rvalue = *this;
  Next();
  return rvalue;
}

bool BasicReader::Iterator::operator==(const Iterator& other) const {
  return queue_ == other.queue_ && status_ == other.status_;
}

bool BasicReader::Iterator::operator!=(const Iterator& other) const {
  return !(*this == other);
}

// ---------------------------------
// BasicReader Implementation
// ---------------------------------

BasicReader::BasicReader(details::EventQueue& queue,
                         const std::size_t max_read_attempt)
    : queue_(std::addressof(queue)), max_read_attempt_(max_read_attempt) {}

BasicReader::BasicReader(const std::string& queue_name,
                         const std::size_t max_read_attempt)
    : queue_(details::system::GetSharedObject<details::EventQueue>(queue_name)),
      max_read_attempt_(max_read_attempt) {}

BasicReader::Iterator BasicReader::begin() const {
  BasicReader::Iterator it(queue_, max_read_attempt_,
                           details::EventQueue::Status::OK);
  return ++it;
}

BasicReader::Iterator BasicReader::end() const {
  return {queue_, max_read_attempt_, details::EventQueue::Status::EMPTY};
}

// ---------------------------------

}  // namespace inspector