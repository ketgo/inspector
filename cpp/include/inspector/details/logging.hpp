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

#include <sstream>

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
  virtual void operator<<(const std::string& message) {}  // NOP
};

/**
 * @brief Get null logger.
 *
 * Get the null logger which does not perform any logging operation. This is the
 * default logger used by the library.
 *
 * @return Reference to the null logger.
 */
inline Logger& NullLogger() {
  static Logger logger_;
  return logger_;
}

namespace details {

/**
 * @brief Handle to the logger passed by the user.
 *
 */
class LoggerHandle {
 public:
  /**
   * @brief Construct a new LoggerHandle object.
   *
   */
  LoggerHandle();

  Logger* operator->() const;
  Logger& operator*() const;
  LoggerHandle& operator=(Logger* logger);

 private:
  Logger* logger_;
};

// ----------------------------
// LoggerHandle Implementation
// ----------------------------

inline LoggerHandle::LoggerHandle() : logger_(std::addressof(NullLogger())) {}

inline Logger* LoggerHandle::operator->() const { return logger_; }

inline Logger& LoggerHandle::operator*() const { return *logger_; }

inline LoggerHandle& LoggerHandle::operator=(Logger* logger) {
  logger_ = logger;
  return *this;
}

// ----------------------------

/**
 * @brief The class `Log` contains the different loggers registered by the user
 * and exposes interface used by the lib to write logs.
 *
 */
class Log {
 public:
  /**
   * @brief Register the given logger for the specified log level.
   *
   * @param level Log level.
   * @param logger Reference to the logger to use for logging.
   */
  static void RegisterLogger(LogLevel level, Logger& logger);

  /**
   * @brief Construct a new Log object.
   *
   * @param level Log level.
   */
  explicit Log(LogLevel level);

  /**
   * @brief Destroy the Log object.
   *
   * The DTOR passes the streamed log to the user specified logger.
   *
   */
  ~Log();

  /**
   * @brief Write the given log message.
   *
   * @tparam T Type of log message.
   * @param message Constant reference to the log message.
   * @returns Reference to the log object.
   */
  template <class T>
  Log& operator<<(const T& message);

 private:
  /**
   * @brief Get the LoggerHandle for given log level.
   *
   */
  static LoggerHandle& LoggerHandleForLevel(LogLevel level);

  Logger& logger_;            // Logger in active use
  std::stringstream stream_;  // Object for streaming logs to string
};

// ------------------------------
// Log Implementation
// ------------------------------

// static
inline LoggerHandle& Log::LoggerHandleForLevel(LogLevel level) {
  // Loggers for the different log levels
  static LoggerHandle loggers_[3];
  return loggers_[static_cast<int>(level)];
}

// --------- public -------------

// static
inline void Log::RegisterLogger(LogLevel level, Logger& logger) {
  LoggerHandleForLevel(level) = std::addressof(logger);
}

inline Log::Log(LogLevel level)
    : logger_(*LoggerHandleForLevel(level)), stream_() {}

inline Log::~Log() { logger_ << stream_.str(); }

template <class T>
Log& Log::operator<<(const T& message) {
  stream_ << message;
  return *this;
}

// ------------------------------

}  // namespace details
}  // namespace inspector

// -----------------------------------------------------
// Convinence Macros for logging
// -----------------------------------------------------

#define LOG_INFO inspector::details::Log(inspector::LogLevel::INFO)

#define LOG_WARN inspector::details::Log(inspector::LogLevel::WARN)

#define LOG_ERROR inspector::details::Log(inspector::LogLevel::ERROR)
