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

#include "examples/cpp/init.hpp"

#include <glog/logging.h>

#include <inspector/config.hpp>
#include <inspector/trace.hpp>
#include <inspector/logging.hpp>

namespace inspector {
namespace examples {
namespace {

// -------------------------------------------------------------
// Code section to add glog as the logger for inspector library.
// -------------------------------------------------------------

class InfoLogger : public Logger {
  void operator<<(const std::string& message) { LOG(INFO) << message; }
};

class WarnLogger : public Logger {
  void operator<<(const std::string& message) { LOG(WARNING) << message; }
};

class ErrorLogger : public Logger {
  void operator<<(const std::string& message) { LOG(ERROR) << message; }
};

/**
 * @brief Register Glog with inspector for logging.
 *
 */
void RegisterGlog() {
  static InfoLogger info;
  static WarnLogger warn;
  static ErrorLogger error;
  RegisterLogger(LogLevel::INFO, info);
  RegisterLogger(LogLevel::WARN, warn);
  RegisterLogger(LogLevel::ERROR, error);
}

}  // namespace

/**
 * @brief Register glog library with the inspector library to enable logging.
 *
 */
void InitInspector() {
  // Remove event queue on exit
  SetRemoveEventQueueOnExit(true);

  // Register Glog
  RegisterGlog();
}

// ----------------------------------------------------------------

}  // namespace examples
}  // namespace inspector