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

namespace inspector {
namespace details {

/**
 * @brief Default event queue system unique name.
 *
 */
constexpr auto kEventQueueSystemUniqueName = "/inspector-56027e94-events";

/**
 * @brief Remove event queue on application exit.
 *
 */
constexpr auto kEventQueueRemoveOnExit = false;

/**
 * @brief Max attempt when consuming or publishing events.
 *
 */
constexpr auto kMaxAttempt = 32;

/**
 * @brief The class `Config` contains configuration settings for the inspector
 * library. It can be used to change the default settings.
 *
 * NOTE: Configuration must be set before any method or classes in the inspector
 * library is used.
 *
 */
class Config {
 public:
  /**
   * @brief Get library config instance.
   *
   */
  static Config& Get();

  std::string queue_system_unique_name;
  bool queue_remove_on_exit;
  std::size_t read_max_attempt;
  std::size_t write_max_attempt;
  bool disable_tracing;

 private:
  /**
   * @brief Construct a new Config object.
   *
   */
  Config();
};

// ----------------------------
// Config Implementation
// ----------------------------

inline Config::Config()
    : queue_system_unique_name(kEventQueueSystemUniqueName),
      queue_remove_on_exit(kEventQueueRemoveOnExit),
      read_max_attempt(kMaxAttempt),
      write_max_attempt(kMaxAttempt),
      disable_tracing(false) {}

// --------- public -----------

// static
inline Config& Config::Get() {
  static Config config;
  return config;
}

// ----------------------------

}  // namespace details
}  // namespace inspector