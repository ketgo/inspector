/**
 * Copyright 2023 Ketan Goyal
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

#include <inspector/config.hpp>

#include <bigcat/circular_queue.hpp>

namespace inspector {
namespace Config {
namespace {

/**
 * @brief Flag to turn on and off capturing of trace events.
 *
 * @returns Reference to the flag.
 */
bool &traceFlag() {
  static bool value = true;
  return value;
}

std::string &queueName() {
  static std::string name = "/inspector-56027e94-events";
  return name;
}

}  // namespace

std::string eventQueueName() { return queueName(); }

void setEventQueueName(const std::string &name) { queueName() = name; }

bool isTraceDisabled() { return !traceFlag(); }

void disableTrace() { traceFlag() = false; }

void enableTrace() { traceFlag() = true; }

}  // namespace Config
}  // namespace inspector