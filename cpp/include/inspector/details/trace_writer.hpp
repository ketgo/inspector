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

#include <bigcat/circular_queue.hpp>

#include <inspector/config.hpp>
#include <inspector/details/event.hpp>
#include <inspector/details/event_queue.hpp>
#include <inspector/details/serializer.hpp>
#include <inspector/details/system.hpp>

namespace inspector {
namespace details {

size_t& threadLocalCounter();

template <class... Args>
void writeEvent(const uint8_t type, const uint8_t category, const char* name,
                const Args&... args) {
  if (Config::isTraceDisable()) {
    return;
  }
  ++(threadLocalCounter());
  auto result =
      eventQueue().tryPublish(sizeof(EventHeader) + storageSize(name, args...));
  if (result.first != bigcat::CircularQueue::Status::OK) {
    return;
  }
  auto* header = reinterpret_cast<EventHeader*>(result.second.data());
  header->type = type;
  header->category = category;
  header->counter = threadLocalCounter();
  header->timestamp =
      std::chrono::system_clock::now().time_since_epoch().count();
  header->pid = getPID();
  header->tid = getTID();
  dump(result.second.data() + sizeof(EventHeader), name, args...);
}

}  // namespace details
}  // namespace inspector