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

namespace inspector {
namespace details {
namespace circular_queue {

/**
 * @brief Enumerated set of possible status.
 *
 */
enum class Status {
  OK = 0,     // Queue is ok for publishing and consumption
  FULL = 1,   // Queue is full so can not publish
  EMPTY = 2,  // Queue is empty so can not consume
};

}  // namespace circular_queue
}  // namespace details
}  // namespace inspector
