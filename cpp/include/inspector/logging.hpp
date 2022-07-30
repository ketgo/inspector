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

#include <inspector/details/logging.hpp>

namespace inspector {

/**
 * @brief Register the given logger for the specified log level.
 *
 * @param level Log level.
 * @param logger Reference to the logger to use for logging.
 */
inline void RegisterLogger(LogLevel level, Logger& logger) {
  details::Log::RegisterLogger(level, logger);
}

}  // namespace inspector