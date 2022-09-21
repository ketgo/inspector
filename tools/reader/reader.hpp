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

#include <atomic>
#include <thread>
#include <vector>

#include <inspector/details/config.hpp>
#include <inspector/details/event_queue.hpp>

#include "tools/reader/event.hpp"
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
  using Buffer = SlidingWindowPriorityQueue<Event>;

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

    Event* operator->();
    Event& operator*();
    Iterator& operator++();
    Iterator operator++(int);
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;

   private:
    Iterator(Buffer& buffer, const std::chrono::microseconds& timeout_ms);
    Iterator(Buffer& buffer, bool timeout);
    void Next();

    std::pair<int64_t, Event> event_;
    Buffer* buffer_;
    std::chrono::microseconds timeout_ms_;
    bool timeout_;
  };

  /**
   * @brief Get the default number of workers used by the reader.
   *
   */
  static constexpr std::size_t DefaultWorkerCount() {
    return 4;  // 4 workers
  }

  static constexpr std::chrono::microseconds DefaultPollingInterval() {
    return std::chrono::microseconds{1000};  // 1ms
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
   * it does not exist then the CTOR will throw an exception. Furthermore, this
   * version of the CTOR creats a blocking reader. A blocking reader will block
   * until a new event is observed when using the iterator interface.
   *
   * @param queue_name Constant reference to the queue name containing trace
   * events. Note that the name used should be unique accross the operating
   * system.
   * @param max_read_attempt Maximum number of attempts to make when reading an
   * event from the queue.
   * @param polling_interval_ms Event queue consumer polling internval in
   * microseconds.
   * @param worker_count Number of background workers reading trace events.
   * @param buffer_window_size Window size used by the internal event buffer.
   */
  Reader(const std::string& queue_name =
             details::Config::Get().queue_system_unique_name,
         const std::size_t max_read_attempt =
             details::Config::Get().max_read_attempt,
         const std::chrono::microseconds& polling_interval_ms =
             DefaultPollingInterval(),
         const std::size_t worker_count = DefaultWorkerCount(),
         const int64_t buffer_window_size = DefaultBufferWindowSize());

  /**
   * @brief Construct a new Reader object.
   *
   * The reader expects the event queue to already exist during construction. If
   * it does not exist then the CTOR will throw an exception. Furthermore, this
   * version of the CTOR creats a non-blocking reader. A non-blocking reader
   * will block until a new event is observed or timeout is reached when using
   * the iterator interface.
   *
   * @param timeout_ms Constant reference to read timeout in microseconds.
   * @param queue_name Constant reference to the queue name containing trace
   * events. Note that the name used should be unique accross the operating
   * system.
   * @param max_read_attempt Maximum number of attempts to make when reading an
   * event from the queue.
   * @param polling_interval_ms Event queue consumer polling internval in
   * microseconds.
   * @param worker_count Number of background workers reading trace events.
   * @param buffer_window_size Window size used by the internal event buffer.
   */
  Reader(const std::chrono::microseconds& timeout_ms,
         const std::string& queue_name =
             details::Config::Get().queue_system_unique_name,
         const std::size_t max_read_attempt =
             details::Config::Get().max_read_attempt,
         const std::chrono::microseconds& polling_interval_ms =
             DefaultPollingInterval(),
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
  /**
   * @brief Starts all the background tasks.
   *
   */
  void Start();

  /**
   * @brief Stops all the running background tasks.
   *
   */
  void Stop();

  details::EventQueue* queue_;
  const std::size_t max_read_attempt_;
  const std::chrono::microseconds polling_interval_ms_;
  std::vector<std::thread> workers_;
  mutable Buffer buffer_;
  mutable std::atomic_bool stop_;
  const std::chrono::microseconds timeout_ms_;
};

// ---------------------------------

}  // namespace inspector