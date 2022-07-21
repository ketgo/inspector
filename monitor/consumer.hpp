
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

#include <inspector/details/common.hpp>

namespace inspector {

/**
 * @brief Event in raw format.
 *
 */
using RawEvent = std::string;

/**
 * @brief The class `Consumer` consumes events from event queue for further
 * processing.
 *
 */
class Consumer {
 public:
  /**
   * @brief Construct a new Consumer object. As part of the CTOR the event queue
   * is also created if it already does not exist.
   *
   * @param remove Flag to indicate removal of the queue on DTOR.
   *
   */
  Consumer(bool remove = false);

  /**
   * @brief Destroy the Consumer object. The event queue is marked for removal
   * if the remove flag was set in the CTOR.
   *
   */
  ~Consumer();

  /**
   * @brief Consume and return a raw event from the queue. If no event is found
   * then an empty event is returned.
   *
   * @param max_attempt Maximum number of attempts to make when consuming event.
   * @returns Consumed raw event.
   */
  RawEvent Consume(
      const std::size_t max_attempt = details::EventQueue::defaultMaxAttempt());

 private:
  bool remove_;
  details::EventQueue* queue_;
};

}  // namespace inspector
