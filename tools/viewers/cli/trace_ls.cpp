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

/**
 * The `trace_ls` utility is a CLI script to print all captured trace events to
 * standard output.
 *
 */

#include <glog/logging.h>
#include <gflags/gflags.h>

#include <iostream>

#include <inspector/trace.hpp>
#include <inspector/trace_event.hpp>

#include "tools/reader/reader.hpp"

DEFINE_int64(timeout, inspector::Reader::defaultTimeout().count(),
             "Read timeout in microseconds when waiting for trace events.");

DEFINE_uint64(buffer_min_window_size, inspector::Reader::defaultMinWindowSize(),
              "Minimum sliding window size of the priority queue used as "
              "buffer to store events");

DEFINE_uint64(buffer_max_window_size, inspector::Reader::defaultMaxWindowSize(),
              "Maximum sliding window size of the priority queue used as "
              "buffer to store events");

namespace inspector {

int main(int argc, char* argv[]) {
  LOG(INFO) << "Loading trace events...";

  Reader reader(std::chrono::microseconds{FLAGS_timeout},
                Reader::defaultPollingInterval(),
                Reader::defaultConsumerCount(), FLAGS_buffer_min_window_size,
                FLAGS_buffer_max_window_size);
  for (auto& event : reader) {
    std::cout << event.toJson() << "\n";
  }

  LOG(INFO) << "Timout.";

  return 0;
}

}  // namespace inspector

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);

  return inspector::main(argc, argv);
}