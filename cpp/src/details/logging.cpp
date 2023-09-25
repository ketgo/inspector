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

#include "cpp/src/details/logging.hpp"

namespace inspector {
namespace details {
namespace {

/**
 * @brief Maximum number of log levels.
 *
 */
auto constexpr kMaxLogLevels = 3;

/**
 * @brief The class `NullLogger` is the default logger which ignores all log
 * messages.
 *
 */
class NullLogger final : public Logger {
 public:
  /**
   * @brief Process the given log message.
   *
   * @param message Constant reference to log message.
   */
  void operator<<(const std::string& message) override {}  // NOP
};

/**
 * @brief The class `LoggerMap` is a map from LogLevel to a Logger instance.
 *
 */
class LoggerMap {
 public:
  /**
   * @brief Get the logger associated with the given log level.
   *
   * @param level Log level.
   * @returns Reference to the logger for the level.
   */
  Logger& get(LogLevel level) { return *loggers_[static_cast<int>(level)]; }

  /**
   * @brief Set the given logger for the given log level.
   *
   * @param level Log level.
   * @param logger Shared pointer to the logger.
   */
  void set(LogLevel level, std::shared_ptr<Logger> logger) {
    loggers_[static_cast<int>(level)] = logger;
  }

  /**
   * @brief Unset the logger for the given log level.
   *
   * @param level Log level.
   */
  void unset(LogLevel level) {
    loggers_[static_cast<int>(level)] = null_logger_;
  }

  /**
   * @brief Get singleton instance of the map.
   *
   * @returns Reference to the map.
   */
  static LoggerMap& instance() {
    static LoggerMap map;
    return map;
  }

 private:
  /**
   * @brief Construct a new Logger Map object.
   *
   */
  LoggerMap() : null_logger_(std::make_shared<NullLogger>()) {
    // Setting the null logger for all levels by default.
    for (size_t i = 0; i < kMaxLogLevels; ++i) {
      loggers_[i] = null_logger_;
    }
  }

  std::shared_ptr<NullLogger> null_logger_;
  std::shared_ptr<Logger> loggers_[kMaxLogLevels];
};

}  // namespace

void registerLogger(LogLevel level, std::shared_ptr<Logger> logger) {
  LoggerMap::instance().set(level, logger);
}

void unregisterLogger(LogLevel level) { LoggerMap::instance().unset(level); }

// ----------
// LogWriter
// ----------

LogWriter::LogWriter(LogLevel level)
    : logger_(LoggerMap::instance().get(level)), stream_() {}

LogWriter::~LogWriter() { logger_ << stream_.str(); }

// ---

}  // namespace details
}  // namespace inspector