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

#include <memory>
#include <sstream>

#include <inspector/logging.hpp>  // Needed for `LogLevel` and `Logger`

namespace inspector {
namespace details {

/**
 * @brief Register the given logger for the specified log level.
 *
 * @param level Log level.
 * @param logger Reference to the logger to use for logging.
 */
void registerLogger(LogLevel level, std::shared_ptr<Logger> logger);

/**
 * @brief Unregister logger for the given log level.
 *
 * The method unregisters any registered logger for thw given log level. After
 * this call any log messages at the given log level will be ignored.
 *
 * @param level Log level for which to unregister logger.
 */
void unregisterLogger(LogLevel level);

/**
 * @brief The class `LogWriter` is responsible for constructing a streamed log
 * message during its life cycle and forwarding it to an appropriate `Logger`
 * instance for further processing.
 *
 */
class LogWriter {
 public:
  /**
   * @brief Construct a new LogWriter object.
   *
   * @param level Log level.
   */
  explicit LogWriter(LogLevel level);

  /**
   * @brief Destroy the LogWriter object.
   *
   * The DTOR passes the streamed log to the user specified logger.
   *
   */
  ~LogWriter();

  /**
   * @brief Process the given log message.
   *
   * @tparam T Type of log message.
   * @param message Constant reference to the log message.
   * @returns Reference to the log object.
   */
  template <class T>
  LogWriter& operator<<(const T& message) {
    stream_ << message;
    return *this;
  }

 private:
  Logger& logger_;            // Logger in active use
  std::stringstream stream_;  // Object for streaming logs to string
};

}  // namespace details
}  // namespace inspector

// -----------------------------------------------------
// Macros for logging
// -----------------------------------------------------

#define LOG_INFO inspector::details::LogWriter(inspector::LogLevel::INFO)

#define LOG_WARN inspector::details::LogWriter(inspector::LogLevel::WARN)

#define LOG_ERROR inspector::details::LogWriter(inspector::LogLevel::ERROR)
