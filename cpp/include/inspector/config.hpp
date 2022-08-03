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

#include <inspector/details/config.hpp>

namespace inspector {

/**
 * @brief The `Config` class contains settings to configure the event writer and
 * reader.
 *
 */
struct Config {
  std::string queue_system_unique_name;
  std::size_t max_attempt;
  bool remove;

  Config(const std::string& queue_system_unique_name_ =
             details::kEventQueueSystemUniqueName,
         const std::size_t max_attempt_ = details::kMaxAttempt,
         bool remove_ = false)
      : queue_system_unique_name(queue_system_unique_name_),
        max_attempt(max_attempt_),
        remove(remove_) {}
};

}  // namespace inspector