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
#include <iostream>

#include <inspector/details/logging.hpp>
#include <inspector/details/system.hpp>

#include "tools/reader/basic_reader.hpp"

namespace inspector {
namespace {

/**
 * @brief Get the event queue where trace events are stored.
 *
 * The method attempts to get the event queue where the trace events are
 * stored. On failure, the method keeps on trying to get the queue until the
 * given max attempt value is reached.
 *
 * @param queue_name Constant reference to the queue name.
 * @param max_attempt Maximum number of attempts to get the queue.
 * @param interval Number of milliseconds to wait before each attempt.
 * @returns Pointer to the event queue.
 */
details::EventQueue* GetEventQueue(const std::string queue_name,
                                   const std::size_t max_attempt,
                                   const std::chrono::milliseconds interval) {
  assert(max_attempt > 0);
  std::size_t attempts = 0;
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

}  // namespace

// ---------------------------------
// Reader Implementation
// ---------------------------------

void Reader::RunWorker() {
  BasicReader reader(*queue_, read_max_attempt_);
  while (true) {
    for (auto&& event : reader) {
      // TODO: Parse the event and push it to priority queue.
      std::cout << event << "\n";
    }
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
}

// ----------- public --------------

Reader::Reader(const std::string& queue_name,
               const std::size_t max_connection_attempt,
               const std::chrono::milliseconds connection_interval,
               const std::size_t read_max_attempt,
               const std::size_t worker_count, const int64_t buffer_window_size)
    : queue_(GetEventQueue(queue_name, max_connection_attempt,
                           connection_interval)),
      read_max_attempt_(read_max_attempt),
      workers_(worker_count),
      buffer_(buffer_window_size) {
  for (auto& worker : workers_) {
    worker = std::thread(&Reader::RunWorker, this);
  }
}

Reader::~Reader() {
  for (auto& worker : workers_) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

}  // namespace inspector