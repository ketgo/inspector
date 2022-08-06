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

#include "utils/strings.hpp"

namespace utils {

std::vector<std::string> Split(const std::string& string,
                               const std::string& delimeter) {
  std::vector<std::string> splits = {};
  std::size_t pos_start = 0, pos_end = 0;

  while ((pos_end = string.find(delimeter, pos_start)) != std::string::npos) {
    std::string token = string.substr(pos_start, pos_end - pos_start);
    splits.push_back(token);
    pos_start = pos_end + delimeter.length();
  }
  std::string token = string.substr(pos_start);
  splits.push_back(token);

  return splits;
}

}  // namespace utils