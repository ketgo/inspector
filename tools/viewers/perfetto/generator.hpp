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

#pragma once

#include <string>

namespace inspector {
namespace tools {

/**
 * @brief Method to generate perfetto trace file from events stored in given
 * storage directoy.
 *
 * @param input_dir Path to storage directory from where to read events.
 * @param output_file Path to output file.
 */
void generatePerfetto(const std::string& input_dir,
                      const std::string output_file);

}  // namespace tools
}  // namespace inspector