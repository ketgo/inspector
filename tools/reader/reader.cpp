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

#include <cassert>
#include <thread>

#include <inspector/details/logging.hpp>
#include <inspector/details/system.hpp>

namespace inspector {

// ---------------------------------
// Reader Implementation
// ---------------------------------

// static
details::EventQueue* Reader::GetEventQueue(
    const std::string queue_name, const std::size_t max_attempt,
    const std::chrono::milliseconds interval) {
  assert(max_attempt > 0);
  auto attempts = 0;
  while (true) {
    try {
      return details::system::GetSharedObject<details::EventQueue>(queue_name);
    } catch (const std::system_error& error) {
      if (error.code().value() != ENOENT || max_attempt <= ++attempts) {
        throw error;
      }
      LOG_ERROR << "Event queue '" << queue_name << "' not found. Retrying in '"
                << interval.count() << " ms'...";
      std::this_thread::sleep_for(interval);
    }
  }
}

// ----------- public --------------

Reader::Reader(const std::string& queue_name, const std::size_t max_attempt,
               const std::size_t max_connection_attempt,
               const std::chrono::milliseconds connection_interval)
    : max_attempt_(max_attempt),
      queue_name_(queue_name),
      queue_(GetEventQueue(queue_name_, max_connection_attempt,
                           connection_interval)) {}

}  // namespace inspector