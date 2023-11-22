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

#include <chrono>

#include <inspector/trace_event.hpp>

namespace inspector {

/**
 * @brief Read a stored trace event from the process shared queue. The method
 * blocks until a trace event is read.
 *
 * @param max_attempt Number of attempts to make for reading a trace event.
 * Default set to 32.
 * @returns An object of type `TraceEvent`.
 */
TraceEvent readTraceEvent(const size_t max_attempt = 32);

}  // namespace inspector