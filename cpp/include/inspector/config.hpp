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

// TODO: Better naming

/**
 * @brief Get the Event Queue Name object
 *
 * @return std::string
 */
inline std::string EventQueueName() {
  return details::Config::Get().queue_system_unique_name;
}

/**
 * @brief Set the Event Queue Name object.
 *
 * @param name
 */
inline void SetEventQueueName(const std::string name) {
  details::Config::Get().queue_system_unique_name = name;
}

/**
 * @brief
 *
 * @return true
 * @return false
 */
inline bool RemoveEventQueueOnExit() {
  return details::Config::Get().queue_remove_on_exit;
}

/**
 * @brief Set the Queue Remove On Exit object
 *
 * @param flag
 */
inline void SetRemoveEventQueueOnExit(bool remove) {
  details::Config::Get().queue_remove_on_exit = remove;
}

/**
 * @brief
 *
 * @return std::size_t
 */
inline std::size_t WriteMaxAttempt() {
  return details::Config::Get().write_max_attempt;
}

/**
 * @brief Set the Write Max Attempt object
 *
 * @param max_attempt
 */
inline void SetWriteMaxAttempt(const std::size_t max_attempt) {
  details::Config::Get().write_max_attempt = max_attempt;
}

/**
 * @brief
 *
 * @return std::size_t
 */
inline std::size_t ReadMaxAttempt() {
  return details::Config::Get().read_max_attempt;
}

/**
 * @brief Set the Write Max Attempt object
 *
 * @param max_attempt
 */
inline void SetReadMaxAttempt(const std::size_t max_attempt) {
  details::Config::Get().read_max_attempt = max_attempt;
}

/**
 * @brief
 *
 * @return true
 * @return false
 */
inline bool IsTraceDisable() { return details::Config::Get().disable_tracing; }

/**
 * @brief
 *
 */
inline void DisableTrace() { details::Config::Get().disable_tracing = true; }

/**
 * @brief
 *
 */
inline void EnableTrace() { details::Config::Get().disable_tracing = false; }

}  // namespace inspector