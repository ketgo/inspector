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

// The file contains common constants and types between the lib and monitor.

#pragma once

#include <inspector/details/circular_queue.hpp>

namespace inspector {
namespace details {

// Size of the event queue in bytes.
constexpr auto kQueueSize = 64 * 1024;  // 64 MB

// Maximum number of producers allowed to concurrently publish events in a
// lock-free manner.
constexpr auto kMaxProducers = 1024;

// Maximum number of consumers allowed to concurrently consume events in a
// lock-free manner.
constexpr auto kMaxConsumers = 1024;

// Queue of trace and metric events published by the different producers.
using EventQueue =
    CircularQueue<char, kQueueSize, kMaxProducers, kMaxConsumers>;

// System unique name of the event queue used for sharing it between different
// processes.
constexpr auto kTraceQueueSystemUniqueName = "/inspector-56027e94-events";

}  // namespace details
}  // namespace inspector