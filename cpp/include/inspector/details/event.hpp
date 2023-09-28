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

#include <cstdint>
#include <cstddef>

#define PACKED __attribute__((packed))

namespace inspector {
namespace details {

/**
 * @brief The data structure `EventHeader` contains all the header information
 * of a single captured trace or metric event using the inspector library.
 *
 */
struct PACKED EventHeader {
  uint8_t type;       //<- Type of event. This field is used to distinguish the
                      // different events.
  uint8_t category;   //<- Category of event.
  size_t counter;     //<- Counter used to track for lost events.
  int64_t timestamp;  //<- Timestamp in nano seconds when the event occurred.
  int32_t pid;  //<- Identifier of the process in which the event occurred or
                // is observed.
  int32_t tid;  //<- Identifier of the thread in which the event occurred or is
                // observed.
};

}  // namespace details
}  // namespace inspector