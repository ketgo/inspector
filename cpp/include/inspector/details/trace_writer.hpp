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
#include <chrono>

#include <bigcat/circular_queue_a.hpp>

#include <inspector/config.hpp>
#include <inspector/details/queue.hpp>
#include <inspector/details/system.hpp>
#include <inspector/details/trace_event.hpp>
#include <inspector/types.hpp>

namespace inspector {
namespace details {

/**
 * @brief Get the thread local counter.
 *
 * @returns Thread local counter value.
 */
size_t& threadLocalCounter();

/**
 * @brief Write a trace event to the process shared queue.
 *
 * @tparam Args Type of debug arguments.
 * @param type Type of trace event.
 * @param name Name of the trace event.
 * @param args Debug arguments.
 */
template <class... Args>
void writeTraceEvent(const event_type_t type, const char* name,
                     const Args&... args) {
  if (Config::isTraceDisabled()) {
    return;
  }
  auto result = eventQueue().publish(traceEventStorageSize(name, args...));
  if (result.first != bigcat::CircularQueueA::Status::OK) {
    return;
  }
  auto event = MutableTraceEvent(result.second.data(), result.second.size());
  event.setType(type);
  event.setCounter(++threadLocalCounter());
  event.setTimestampNs(
      std::chrono::system_clock::now().time_since_epoch().count());
  event.setPid(getPID());
  event.setTid(getTID());
  event.appendDebugArgs(name, args...);
}

}  // namespace details
}  // namespace inspector