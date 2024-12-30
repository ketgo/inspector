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
 * @brief Start recorder.
 *
 * @param out Path to output directory to store trace and metric records.
 * @param block Invoke a blocking call.
 */
void startRecorder(const std::string& out, const bool block = false);

/**
 * @brief Stop recorder.
 *
 * @param block Block until the recorder has terminated.
 */
void stopRecorder(const bool block = false);

}  // namespace tools
}  // namespace inspector