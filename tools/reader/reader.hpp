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

#include <thread>
#include <vector>

#include <inspector/details/config.hpp>
#include <inspector/details/event_queue.hpp>

#include "tools/reader/priority_queue.h"

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
  // Buffer to store trace events in chronological order
  using Buffer = SlidingWindowPriorityQueue<std::string>;

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
    Iterator(Buffer& buffer);
    void Next();

    Buffer* buffer_;
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
   * @brief Get the default number of workers used by the reader.
   *
   */
  static constexpr std::size_t DefaultWorkerCount() {
    return 4;  // 4 workers
  }

  /**
   * @brief Get the default buffer window size.
   *
   */
  static constexpr int64_t DefaultBufferWindowSize() {
    return 2000000000;  // 2s
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
   * @param max_connection_attempt Maximum number of event queue connection
   * attempts.
   * @param connection_interval Number of milliseconds to wait between each
   * connection attempt.
   * @param read_max_attempt Maximum number of attempts to make when reading an
   * event from the queue.
   * @param thread_count Number of background workers reading trace events.
   * @param buffer_window_size Window size used by the internal event buffer.
   */
  Reader(
      const std::string& queue_name =
          details::Config::Get().queue_system_unique_name,
      const std::size_t max_connection_attempt = DefaultMaxConnectionAttempt(),
      const std::chrono::milliseconds connection_interval =
          DefaultConnectionAttemptInterval(),
      const std::size_t read_max_attempt =
          details::Config::Get().read_max_attempt,
      const std::size_t worker_count = DefaultWorkerCount(),
      const int64_t buffer_window_size = DefaultBufferWindowSize());

  /**
   * @brief Destroy the Reader object.
   *
   */
  ~Reader();

  // NOTE: We break our naming convention for the methods `begin` and `end` in
  // order to support the standard `for` loop expressions, i.e. `for(auto& x:
  // reader)`.

  Iterator begin() const;
  Iterator end() const;

 private:
  void RunWorker();

  details::EventQueue* queue_;
  const std::size_t read_max_attempt_;
  std::vector<std::thread> workers_;
  Buffer buffer_;
};

// ---------------------------------

}  // namespace inspector