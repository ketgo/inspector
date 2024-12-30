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

#include "tools/recorder/storage_collector.hpp"

namespace inspector {
namespace tools {
namespace {

/**
 * @brief Block size in bytes to use for storage.
 *
 */
constexpr auto kBlockSize = 1024UL * 1024UL * 100UL;  // 100MB

}  // namespace

StorageCollector::StorageCollector(const std::string& out_dir)
    : writer_(out_dir, kBlockSize) {}

void StorageCollector::process(const TraceEvent& trace_event) {
  const auto span = trace_event.span();
  writer_.write({trace_event.timestampNs(), span.first, span.second});
}

void StorageCollector::flush() { writer_.flush(); }

}  // namespace tools
}  // namespace inspector