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

// Size of the trace queue in bytes.
constexpr auto kTraceQueueSize = 64 * 1024;  // 64 MB

// Maximum number of trace producers that can concurrently publish to the
// trace queue in a lock-free manner.
constexpr auto kMaxTraceProducers = 1024;

// Maximum number of trace monitors that can concurrently consume from the
// trace queue in a lock-free manner.
constexpr auto kMaxTraceMonitors = 1024;

// Queue of trace events published by the different trace producers.
using TraceQueue = CircularQueue<unsigned char, kTraceQueueSize,
                                 kMaxTraceProducers, kMaxTraceMonitors>;

// System unique name of the trace queue used for sharing the queue between
// different processes.
constexpr auto kTraceQueueSystemUniqueName = "/inspector-56027e94-trace_events";

}  // namespace details
}  // namespace inspector