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

#include <inspector/logging.hpp>
#include <inspector/details/logging.hpp>

namespace inspector {

void registerLogger(LogLevel level, std::shared_ptr<Logger> logger) {
  details::registerLogger(level, logger);
}

void unregisterLogger(LogLevel level) { details::unregisterLogger(level); }

void unregisterAllLoggers() {
  unregisterLogger(LogLevel::INFO);
  unregisterLogger(LogLevel::WARN);
  unregisterLogger(LogLevel::ERROR);
}

}  // namespace inspector