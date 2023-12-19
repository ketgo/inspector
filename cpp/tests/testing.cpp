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

#include "cpp/tests/testing.hpp"

#include <bigcat/circular_queue_a.hpp>
#include <inspector/config.hpp>
#include <inspector/details/queue.hpp>

namespace inspector {
namespace testing {

void removeEventQueue() {
  bigcat::CircularQueueA::remove(Config::eventQueueName());
}

void emptyEventQueue() {
  while (1) {
    auto result = details::eventQueue().consume(1024);
    if (result.first == bigcat::CircularQueueA::Status::EMPTY) {
      return;
    }
  };
}

}  // namespace testing
}  // namespace inspector