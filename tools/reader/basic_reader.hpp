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

#include <string>

#include <inspector/details/event_queue.hpp>

namespace inspector {

/**
 * @brief The class `BasicReader` can be used to consume events from the shared
 * queue containing trace or metrics events. It exposes the standard iterator
 * interface which can be used in a `for` loop to consume the events. It is used
 * internally by the high level reader to consume trace events concurrently.
 *
 */
class BasicReader {
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
    friend BasicReader;

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
    const std::size_t max_attempt_;
    details::EventQueue::Status status_;
    std::string event_;
  };

  /**
   * @brief Construct a new BasicReader object.
   *
   * @param queue Reference to the event queue containing trace events.
   * @param max_attempt Maximum number of attempts to make when reading an event
   * from the queue.
   */
  BasicReader(details::EventQueue& queue, const std::size_t max_attempt);

  // NOTE: We break our naming convention for the methods `begin` and `end` in
  // order to support the standard `for` loop expressions, i.e. `for(auto& x:
  // reader)`.

  Iterator begin() const;
  Iterator end() const;

 private:
  details::EventQueue* queue_;
  const std::size_t max_attempt_;
};

}  // namespace inspector