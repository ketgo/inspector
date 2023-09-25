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
#include <string>

namespace inspector {

/**
 * @brief Enumerated set of logging levels.
 *
 */
enum class LogLevel {
  INFO = 0,
  WARN = 1,
  ERROR = 2,
};

/**
 * @brief Abstract logger class.
 *
 * A derived class must be registered by the user in order to enable logging in
 * the inspector lib. The class acts as an adapter to any native logging lib
 * used by the user.
 *
 */
class Logger {
 public:
  virtual ~Logger() = default;

  /**
   * @brief Process a given log message.
   *
   * @param message Constant reference to log message.
   */
  virtual void operator<<(const std::string& message) = 0;
};

/**
 * @brief Register the given logger for the specified log level.
 *
 * @param level Log level.
 * @param logger Shared pointer to the logger to use for logging.
 */
void registerLogger(LogLevel level, std::shared_ptr<Logger> logger);

/**
 * @brief Unregister any registered logger for the given log level.
 *
 * The method unregisters any registered logger for the given log level. After
 * this call log messages at the log level will be ignored.
 *
 * @param level Log Level for which to unregister logger.
 */
void unregisterLogger(LogLevel level);

/**
 * @brief Unregister all registered loggers.
 *
 */
void unregisterAllLoggers();

}  // namespace inspector