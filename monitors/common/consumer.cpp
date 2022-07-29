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

#include "monitors/common/consumer.hpp"

#include <thread>

#include <inspector/details/shared_object.hpp>

namespace inspector {

// --------------------------------
// Basic Consumer
// --------------------------------

Consumer::Consumer(bool remove)
    : remove_(remove),
      queue_(details::shared_object::GetOrCreate<details::EventQueue>(
          details::kTraceQueueSystemUniqueName)) {}

std::string Consumer::Consume(const std::size_t max_attempts) {
  details::EventQueue::ReadSpan span;
  auto result = queue_->Consume(span, max_attempts);
  if (result != details::EventQueue::Result::SUCCESS) {
    return {};
  }
  return {span.Data(), span.Size()};
}

Consumer::~Consumer() {
  if (remove_) {
    details::shared_object::Remove(details::kTraceQueueSystemUniqueName);
  }
}

// --------------------------------
// Periodic Consumer
// --------------------------------

PeriodicConsumer::PeriodicConsumer(const std::chrono::microseconds& period,
                                   const std::size_t max_attempt, bool remove)
    : period_(period), max_attempt_(max_attempt), consumer_(remove) {}

template <class EventHandler>
void PeriodicConsumer::Start(EventHandler& handler) {
  while (true) {
    auto event = consumer_.Consume(max_attempt_);
    if (!handler(event)) {
      break;
    }
    std::this_thread::sleep_for(period_);
  }
}

}  // namespace inspector
