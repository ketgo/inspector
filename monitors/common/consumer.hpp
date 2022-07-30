
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

#include <chrono>
#include <string>
#include <vector>

#include <inspector/details/config.hpp>

namespace inspector {

/**
 * @brief Event in raw format.
 *
 */
using RawEvent = std::string;

// --------------------------------
// Basic Consumer
// --------------------------------

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
  explicit Consumer(bool remove = false);

  /**
   * @brief Destroy the Consumer object. The event queue is marked for removal
   * if the remove flag was set in the CTOR.
   *
   */
  ~Consumer();

  /**
   * @brief Consume and return a batch of events from the queue. If no events
   * are found then an empty batch is returned.
   *
   * @param batch_size Maximum number of events to consume in a batch.
   * @param max_attempt Maximum number of attempts to make when consuming event.
   * @returns Consumed raw event.
   */
  std::vector<std::string> Consume(
      const std::size_t batch_size,
      const std::size_t max_attempt = details::EventQueue::defaultMaxAttempt());

 private:
  bool remove_;
  details::EventQueue* queue_;
};

// --------------------------------
// Periodic Consumer
// --------------------------------

/**
 * @brief The class `PeriodicConsumer` consumes events from event queue for
 * further processing periodically with specified time interval.
 *
 */
class PeriodicConsumer {
 public:
  /**
   * @brief Construct a new PeriodicConsumer object.
   *
   * @param period Constant reference to the duration in milliseconds to wait
   * between each consume attempt.
   * @param batch_size Maximum number of events to consume in a batch.
   * @param max_attempt Maximum number of attempts to make when consuming event.
   * @param remove Flag to indicate removal of the queue on DTOR.
   */
  PeriodicConsumer(
      const std::chrono::microseconds& period, const std::size_t batch_size,
      const std::size_t max_attempt = details::EventQueue::defaultMaxAttempt(),
      bool remove = false);

  /**
   * @brief Start the periodic consumer.
   *
   * The method is blocking and will call the given event handler on successful
   * consumption of a batch of events from the queue. If the handler returns
   * `false` then the consumer stops, otherwise the execution continues. Thus
   * the event handler is required to be a callable taking the consumed events
   * as input and returning a boolean.
   *
   * @tparam EventHandler Event handler type.
   * @param handler Reference to the event handler.
   */
  template <class EventHandler>
  void Start(EventHandler& handler);

 private:
  const std::chrono::microseconds period_;
  const std::size_t batch_size_;
  const std::size_t max_attempt_;
  Consumer consumer_;
};

}  // namespace inspector
