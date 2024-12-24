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
#include <fstream>

#include <inspector/trace.hpp>
#include <inspector/trace_event.hpp>

#include "tools/common/reader/reader.hpp"

DEFINE_int64(timeout, inspector::Reader::defaultTimeout().count(),
             "Read timeout in microseconds when waiting for trace events.");

DEFINE_uint64(buffer_min_window_size, inspector::Reader::defaultMinWindowSize(),
              "Minimum sliding window size of the priority queue used as "
              "buffer to store events");

DEFINE_uint64(buffer_max_window_size, inspector::Reader::defaultMaxWindowSize(),
              "Maximum sliding window size of the priority queue used as "
              "buffer to store events");

DEFINE_string(
    out, "stdout",
    "Path to the output file for storing captured events. When set to 'stdout' "
    "then the events will be printed to standard output. Default set to "
    "'stdout'.");

namespace inspector {

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG_IF(FATAL, FLAGS_out.empty()) << "No output file provided.";

  std::ofstream file;
  if (FLAGS_out != "stdout") {
    file.open(FLAGS_out);
  }
  std::ostream& out = FLAGS_out == "stdout" ? std::cout : file;
  LOG_IF(FATAL, !out) << "Unable to open output stream";

  LOG(INFO) << "Loading trace events...";

  Reader reader(std::chrono::microseconds{FLAGS_timeout},
                Reader::defaultPollingInterval(),
                Reader::defaultConsumerCount(), FLAGS_buffer_min_window_size,
                FLAGS_buffer_max_window_size);
  for (auto& event : reader) {
    out << event.toJson() << "\n";
  }

  LOG(INFO) << "Timeout.";

  return 0;
}

}  // namespace inspector

int main(int argc, char* argv[]) { return inspector::main(argc, argv); }