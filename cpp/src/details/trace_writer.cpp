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

#include "cpp/src/details/trace_writer.hpp"

#include <inspector/config.hpp>

#include "cpp/src/details/logging.hpp"

namespace inspector {
namespace details {
namespace {

/**
 * @brief Get the circular queue configuration.
 *
 */
bigcat::CircularQueue::Config queueConfig() {
  return bigcat::CircularQueue::Config{
      .buffer_size = 8 * 1024 * 1024,  // 8MB
      .max_producers = 1024,
      .max_consumers = 1024,
      .timeout_ns = 30000000000,  // 30s
      .start_marker = 811347036,  // "\\,\\0"
  };
}

}  // namespace

// ----------
// TraceWriter
// ----------

// static
TraceWriter& TraceWriter::instance() {
  static TraceWriter writer_;
  return writer_;
}

TraceWriter::~TraceWriter() {}

void TraceWriter::write(const std::string& trace_event) {
  if (Config::isTraceDisable()) {
    return;
  }
  auto result = queue_.tryPublish(trace_event);
  if (result != bigcat::CircularQueue::Status::OK) {
    // TODO: Need an approach to handle dropping of the trace event.
    LOG_ERROR << "Event Queue Full: Unable to write trace event.";
  }
}

TraceWriter::TraceWriter()
    : queue_(bigcat::CircularQueue::open(Config::eventQueueName(),
                                         queueConfig())) {}

// ----------

}  // namespace details
}  // namespace inspector