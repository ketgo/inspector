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

#include "monitor/consumer.hpp"

#include <inspector/details/shared_object.hpp>

namespace inspector {

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

}  // namespace inspector
