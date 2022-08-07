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

#include <vector>
#include <string>

namespace utils {

/**
 * @brief Split a given string on delimeter into one or more sub-strings.
 *
 * @param string Constant reference to the string to split.
 * @param delimeter Constant reference to the delimeter.
 * @returns Vector of split strings.
 */
std::vector<std::string> Split(const std::string& string,
                               const std::string& delimeter);

/**
 * @brief Split a given string once into two sub-strings on the first found
 * delimeter.
 *
 * @param string Constant reference to the string to split.
 * @param delimeter Constant reference to the delimeter.
 * @returns Pair of substrings.
 */
std::pair<std::string, std::string> SplitOnce(const std::string& string,
                                              const std::string& delimeter);

}  // namespace utils