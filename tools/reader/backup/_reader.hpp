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

#include <cassert>
#include <cstring>
#include <thread>

#include <inspector/details/config.hpp>
#include <inspector/details/event_queue.hpp>
#include <inspector/details/logging.hpp>
#include <inspector/details/system.hpp>

namespace inspector {

/**
 * @brief The class `Reader` can be used to consume events from the shared queue
 * containing trace or metrics events. It exposes the standard iterator
 * interface which can be used in a `for` loop to consume the events.
 *
 */
class Reader {
 public:
  /**
   * @brief Iterator to iterate over events in the event queue.
   *
   * The Iterator class is a forward-only direction iterator. It consumes events
   * from the event queue while iterating.
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
    Iterator(const details::EventQueue* queue, const std::size_t max_attempt,
             details::EventQueue::Status status);

    void Next();

    details::EventQueue* queue_;
    std::size_t max_attempt_;
    details::EventQueue::Status status_;
    std::string event_;
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
// Reader::Iterator Implementation
// ---------------------------------

inline Reader::Iterator::Iterator(const details::EventQueue* queue,
                                  const std::size_t max_attempt,
                                  details::EventQueue::Status status)
    : queue_(const_cast<details::EventQueue*>(queue)),
      max_attempt_(max_attempt),
      status_(status) {}

inline void Reader::Iterator::Next() {
  details::EventQueue::ReadSpan span;
  status_ = queue_->Consume(span, max_attempt_);
  if (status_ == details::EventQueue::Status::OK) {
    event_ = {span.Data(), span.Size()};
  }
}

// ------------ public -------------

inline Reader::Iterator::Iterator()
    : queue_(nullptr),
      max_attempt_(0),
      status_(details::EventQueue::Status::OK) {}

inline std::string* Reader::Iterator::operator->() { return &event_; }

inline std::string& Reader::Iterator::operator*() { return event_; }

inline Reader::Iterator& Reader::Iterator::operator++() {
  Next();
  return *this;
}

inline Reader::Iterator Reader::Iterator::operator++(int) {
  Iterator rvalue = *this;
  Next();
  return rvalue;
}

inline bool Reader::Iterator::operator==(const Iterator& other) const {
  return queue_ == other.queue_ && status_ == other.status_;
}

inline bool Reader::Iterator::operator!=(const Iterator& other) const {
  return !(*this == other);
}

// ---------------------------------
// Reader Implementation
// ---------------------------------

// static
inline details::EventQueue* Reader::GetEventQueue(
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

inline Reader::Reader(const std::string& queue_name,
                      const std::size_t max_attempt,
                      const std::size_t max_connection_attempt,
                      const std::chrono::milliseconds connection_interval)
    : max_attempt_(max_attempt),
      queue_name_(queue_name),
      queue_(GetEventQueue(queue_name_, max_connection_attempt,
                           connection_interval)) {}

inline Reader::Iterator Reader::begin() const {
  Reader::Iterator it(queue_, max_attempt_, details::EventQueue::Status::OK);
  return ++it;
}

inline Reader::Iterator Reader::end() const {
  return {queue_, max_attempt_, details::EventQueue::Status::EMPTY};
}

// ---------------------------------

}  // namespace inspector