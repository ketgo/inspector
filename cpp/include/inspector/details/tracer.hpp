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

#include <inspector/trace_event.hpp>
#include <inspector/writer.hpp>

namespace inspector {
namespace details {

/**
 * @brief Get the trace writer.
 *
 * @return Reference to the trace writer.
 */
inline Writer& TraceWriter() {
  static Writer writer;
  return writer;
}

/**
 * @brief Write the given trace event onto the event queue.
 *
 * @param event Constant reference to the trace event.
 */
inline void WriteTraceEvent(const TraceEvent& event) {
  TraceWriter().Write(event.String());
}

/**
 * @brief Construct and write a trace event with the given arguments.
 *
 */
template <class... Args>
inline void WriteTraceEvent(const char type, const std::string& name,
                            const Args&... args) {
  TraceEvent event(type, name);
  event.SetArgs(args...);
  WriteTraceEvent(event);
}

}  // namespace details
}  // namespace inspector