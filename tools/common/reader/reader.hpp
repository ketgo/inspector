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
#include <chrono>
#include <thread>
#include <vector>

#include <inspector/trace_event.hpp>

#include "tools/common/reader/priority_queue.hpp"

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
 */
class Reader {
  using event_queue_t = details::SlidingWindowPriorityQueue<TraceEvent>;

 public:
  /**
   * @brief Iterator to iterate over trace events in the event queue.
   *
   * The Iterator class is a forward-only direction iterator. It consumes trace
   * events from the priority queue while iterating.
   *
   */
  class Iterator {
    // Needed to access private CTOR
    friend Reader;

   public:
    Iterator();

    // Standard forward iterator interface

    TraceEvent* operator->();
    TraceEvent& operator*();
    Iterator& operator++();
    Iterator operator++(int);
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;

   private:
    Iterator(event_queue_t& queue, const bool closed);
    void next();

    event_queue_t::value_type value_;
    event_queue_t* queue_;
    bool closed_;
  };

  /**
   * @brief Default number of consumers.
   *
   */
  static constexpr size_t defaultConsumerCount() { return 4; }

  /**
   * @brief Defult min slidding window size.
   *
   */
  static constexpr duration_t defaultMinWindowSize() {
    return 1;  // 1ns
  }

  /**
   * @brief Defult min slidding window size.
   *
   */
  static constexpr duration_t defaultMaxWindowSize() {
    return 60000000000;  // 60s
  }

  /**
   * @brief Default read timeout in microseconds.
   *
   */
  static constexpr std::chrono::microseconds defaultTimeout() {
    return std::chrono::microseconds{5000000};  // 5s
  }

  /**
   * @brief Default polling interval in microseconds.
   *
   */
  static constexpr std::chrono::microseconds defaultPollingInterval() {
    return std::chrono::microseconds{1000};  // 1000us
  }

  /**
   * @brief Construct a new Reader object.
   *
   * @param timeout_us Read timeout in microseconds when waiting for trace
   * events.
   * @param polling_interval_us Event queue consumer polling interval in
   * microseconds.
   * @param num_consumers Number of concurrent consumers used be the reader.
   * @param min_window_size Minimum sliding window size of the priority queue
   * used as buffer to store events.
   * @param max_window_size Maximum sliding window size of the priority queue
   * used as buffer to store events.
   */
  Reader(const std::chrono::microseconds timeout_us = defaultTimeout(),
         const std::chrono::microseconds polling_interval_us =
             defaultPollingInterval(),
         const size_t num_consumers = defaultConsumerCount(),
         const duration_t min_window_size = defaultMinWindowSize(),
         const duration_t max_window_size = defaultMaxWindowSize());

  /**
   * @brief Destroy the Reader object.
   *
   */
  ~Reader();

  Iterator begin() const;
  Iterator end() const;

 private:
  std::chrono::microseconds timeout_us_;
  std::chrono::microseconds polling_interval_us_;
  std::vector<std::thread> consumers_;
  mutable event_queue_t queue_;
  std::atomic_size_t count_;
  std::atomic_bool stop_;
};

}  // namespace inspector