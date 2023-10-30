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
namespace Config {

/**
 * @brief Get the name of the process shared event queue used by the inspector
 * library to publish trace events for consumption by the trace reader.
 *
 * @returns System unique name of the shared circular queue.
 */
std::string eventQueueName();

/**
 * @brief Set the name of the process shared event queue used by the inspector
 * library to publish trace events for consumption by the trace reader.
 *
 * @param name Constant reference to the name.
 */
void setEventQueueName(const std::string& name);

/**
 * @brief Check if tracing is disabled.
 *
 * @returns `true` if disabled else `false`.
 */
bool isTraceDisabled();

/**
 * @brief Disable capturing of all trace events.
 *
 */
void disableTrace();

/**
 * @brief Enable capturing of all trace events.
 *
 */
void enableTrace();

}  // namespace Config
}  // namespace inspector