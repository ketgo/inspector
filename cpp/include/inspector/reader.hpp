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

#include <iostream>

#include <inspector/config.hpp>
#include <inspector/details/iterator.hpp>
#include <inspector/details/logging.hpp>
#include <inspector/details/shared_object.hpp>

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
   * @brief Set reader configuration.
   *
   * NOTE: Reader configuration must be set before any method or classes in the
   * inspector is used.
   *
   * @param config Constant reference to the new configuration.
   */
  static void SetConfig(const Config& config);

  /**
   * @brief Construct a new Reader object.
   *
   */
  Reader();

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
  /**
   * @brief Get writer configuration instance.
   *
   */
  static Config& ConfigInstance();

  const bool remove_;
  const std::size_t max_attempt_;
  const std::string queue_name_;
  details::EventQueue* queue_;
};

// ---------------------------------
// Reader Implementation
// ---------------------------------

// static
inline Config& Reader::ConfigInstance() {
  static Config config;
  return config;
}

// ------------- public --------------

// static
inline void Reader::SetConfig(const Config& config) {
  ConfigInstance() = config;
}

inline Reader::Reader()
    : remove_(ConfigInstance().remove),
      max_attempt_(ConfigInstance().max_attempt),
      queue_name_(ConfigInstance().queue_system_unique_name),
      queue_(details::shared_object::GetOrCreate<details::EventQueue>(
          queue_name_)) {}

inline Reader::~Reader() {
  if (remove_) {
    LOG_INFO << "Marking the shared event queue for removal.";
    details::shared_object::Remove(queue_name_);
  }
}

details::Iterator Reader::begin() const {
  details::Iterator it(queue_, max_attempt_, details::EventQueue::Status::OK);
  return ++it;
}

details::Iterator Reader::end() const {
  return {queue_, max_attempt_, details::EventQueue::Status::EMPTY};
}

// ---------------------------------

}  // namespace inspector