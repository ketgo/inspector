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

#include <inspector/details/queue.hpp>

#include <atomic>

#include <inspector/config.hpp>
#include <inspector/details/logging.hpp>

namespace inspector {
namespace details {
namespace {

/**
 * @brief Get the circular queue configuration.
 *
 */
bigcat::CircularQueue::Config queueConfig() {
  bigcat::CircularQueue::Config config;
  config.buffer_size = 8 * 1024 * 1024;  // 8MB
  config.max_producers = 1024;
  config.max_consumers = 1024;
  config.timeout_ns = 30000000000;  // 30s
  config.start_marker = 811347036;  // "\\,\\0"
  return config;
}

}  // namespace

bigcat::CircularQueue& eventQueue() {
  static bigcat::CircularQueue queue =
      bigcat::CircularQueue::open(Config::eventQueueName(), queueConfig());
  return queue;
}

}  // namespace details
}  // namespace inspector