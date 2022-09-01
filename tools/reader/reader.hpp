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

#include <inspector/details/config.hpp>
#include <inspector/details/event_queue.hpp>

namespace inspector {

/**
 * @brief The class `Reader` can be used to consume trace and metric events
 * generated using the inspector library.
 *
 * The Reader consumes trace and metric events from the shared event queue used
 * by the inspector library. It exposes the standard iterator interface which
 * can be used in a `for` loop to read the events. Internally, the reader spawns
 * multiple basic reader instances running concurrently in a thread pool. Each
 * basic reader consumes events from the shared queue and forwards them onto an
 * internal chronologically ordered priority queue. The iterator interface in
 * turn pops events from this priority queue.
 *
 */
class Reader {
 public:
  /**
   * @brief Iterator to iterate over events in the event queue.
   *
   * The Iterator class is a forward-only direction iterator. It consumes events
   * from the prioirty queue while iterating.
   *
   */
  class Iterator {
    // Needed to access private CTOR
    friend Reader;

   public:
    Iterator();

    // Standard forward iterator interface

    std::string* operator->();
    std::string& operator*();
    Iterator& operator++();
    Iterator operator++(int);
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;

   private:
    void Next();
  };

  /**
   * @brief Get the default max connection attempts for the event queue.
   *
   */
  static constexpr std::size_t DefaultMaxConnectionAttempt() {
    return 30;  // 30 attempts
  }

  /**
   * @brief Get the default number of milliseconds to wait before re-attempting
   * event queue connection.
   *
   */
  static constexpr std::chrono::milliseconds
  DefaultConnectionAttemptInterval() {
    return std::chrono::milliseconds{1000};  // 1s
  }

  /**
   * @brief Construct a new Reader object.
   *
   * The reader expects the event queue to already exist during construction. If
   * it does not exist then the CTOR will repeatedly try to connect with the
   * queue until success or max connection attempts is reached.
   *
   * @param queue_name Constant reference to the queue name containing trace
   * events. Note that the name used should be unique accross the operating
   * system.
   * @param max_attempt Maximum number of attempts to make when reading an event
   * from the queue.
   * @param max_connection_attempt Maximum number of event queue connection
   * attempts.
   * @param connection_interval Number of milliseconds to wait between each
   * connection attempt.
   */
  Reader(
      const std::string& queue_name =
          details::Config::Get().queue_system_unique_name,
      const std::size_t max_attempt = details::Config::Get().read_max_attempt,
      const std::size_t max_connection_attempt = DefaultMaxConnectionAttempt(),
      const std::chrono::milliseconds connection_interval =
          DefaultConnectionAttemptInterval());

  // NOTE: We break our naming convention for the methods `begin` and `end` in
  // order to support the standard `for` loop expressions, i.e. `for(auto& x:
  // reader)`.

  Iterator begin() const;
  Iterator end() const;

 private:
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
  static details::EventQueue* GetEventQueue(
      const std::string queue_name, const std::size_t max_attempt,
      const std::chrono::milliseconds interval);

  const std::size_t max_attempt_;  // Max number of read attempts
  const std::string queue_name_;
  details::EventQueue* queue_;
};

// ---------------------------------

}  // namespace inspector