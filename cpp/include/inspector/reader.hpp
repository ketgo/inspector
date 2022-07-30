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

#include <inspector/details/iterator.hpp>
#include <inspector/details/logging.hpp>
#include <inspector/details/shared_object.hpp>

namespace inspector {

/**
 * @brief The class `Reader` can be used to consume events from the shared queue
 * containing tracing or metrics events. It exposes the standard iterator
 * interface which can be used in a for loop to consume events.
 *
 */
class Reader {
 public:
  /**
   * @brief Construct a new Reader object.
   *
   * @param max_attempt Maximum number of attempts to make when consuming event.
   * @param remove Flag for removal of the queue on DTOR.
   */
  Reader(
      const std::size_t max_attempt = details::EventQueue::defaultMaxAttempt(),
      const bool remove = false);

  /**
   * @brief Destroy the Reader object.
   *
   * The DTOR marks the event queue for removal if the remove flag was set in
   * the CTOR.
   *
   */
  ~Reader();

  // NOTE: We break our naming convention for the methods `begin` and `end` in
  // order to support the standard `for` loop expressions, i.e. `for(auto& x:
  // reader)`.

  details::Iterator begin() const;
  details::Iterator end() const;

 private:
  const std::size_t max_attempt_;
  const bool remove_;
  details::EventQueue* queue_;
};

// ---------------------------------
// Reader implementation
// ---------------------------------

inline Reader::Reader(const std::size_t max_attempt, const bool remove)
    : max_attempt_(max_attempt),
      remove_(remove),
      queue_(details::shared_object::GetOrCreate<details::EventQueue>(
          details::kTraceQueueSystemUniqueName)) {}

inline Reader::~Reader() {
  if (remove_) {
    LOG_INFO << "Removing shared event queue...";
    details::shared_object::Remove(details::kTraceQueueSystemUniqueName);
  }
}

// ---------------------------------

}  // namespace inspector