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

#include "tests/cpp/trace_event.hpp"

#include "utils/strings.hpp"

namespace inspector {
namespace testing {

// static
TraceEvent TraceEvent::Parse(const std::string& data) {
  TraceEvent event;
  char delimiter;
  std::istringstream stream(data);
  stream >> event.timestamp >> delimiter >> event.pid >> delimiter >>
      event.tid >> delimiter >> event.type >> delimiter >> event.name;
  auto splits = utils::SplitOnce(event.name, "|");
  event.name = splits.first;
  event.payload = splits.second;
  return event;
}

}  // namespace testing
}  // namespace inspector