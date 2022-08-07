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
namespace testing {

/**
 * @brief Parsed trace event data structure.
 *
 */
struct TraceEvent {
  int64_t timestamp;
  int32_t pid;
  int32_t tid;
  char type;
  std::string name;
  std::string payload;

  /**
   * @brief Get parsed trace event from given string.
   *
   */
  static TraceEvent Parse(const std::string& data);
};

}  // namespace testing
}  // namespace inspector