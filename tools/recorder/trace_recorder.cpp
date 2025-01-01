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

#include "tools/recorder/trace_recorder.hpp"

#include <inspector/trace_reader.hpp>

namespace inspector {
namespace tools {
namespace {

/**
 * @brief Name of recorder.
 *
 */
const auto kRecorderName = "TraceRecorder";

}  // namespace

TraceRecorder::TraceRecorder(const std::shared_ptr<CollectorBase>& collector)
    : RecorderBase(kRecorderName), collector_(collector) {}

void TraceRecorder::record() {
  while (true) {
    auto event = readTraceEvent();
    if (event.isEmpty()) {
      break;
    }
    collector_->process(event);
  }
}

}  // namespace tools
}  // namespace inspector