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

#include <cstdint>

namespace inspector {
namespace details {

/**
 * @brief Common header expected in all the trace or metric events.
 *
 */
struct __attribute__((packed)) EventHeader {
  // Type of event. This field is used to distinguish different events.
  uint8_t type;
  // Timestamp in nano seconds when the event occured.
  int64_t timestamp;
  // Identifier of the process in which the event occured or is observed.
  int32_t pid;
  // Identifier of the thread in which the event occured or is observed.
  int32_t tid;
};

}  // namespace details
}  // namespace inspector