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

#include <inspector/config.hpp>

namespace inspector {
namespace details {
namespace {

/**
 * @brief Get the circular queue configuration.
 *
 */
bigcat::CircularQueue::Config queueConfig() {
  return bigcat::CircularQueue::Config{
      .buffer_size = 8 * 1024 * 1024,  // 8MB
      .max_producers = 1024,
      .max_consumers = 1024,
      .timeout_ns = 30000000000,  // 30s
      .start_marker = 811347036,  // "\\,\\0"
  };
}

}  // namespace

bigcat::CircularQueue& eventQueue() {
  static bigcat::CircularQueue queue =
      bigcat::CircularQueue::open(Config::eventQueueName(), queueConfig());
  return queue;
}

}  // namespace details
}  // namespace inspector