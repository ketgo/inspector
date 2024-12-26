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
 * The `storage_trace_ls` utility is a CLI script to print all captured trace
 * events to standard output.
 *
 */

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <cstring>
#include <fstream>
#include <inspector/trace.hpp>
#include <inspector/trace_event.hpp>
#include <iostream>

#include "tools/common/storage/storage.hpp"

DEFINE_string(in, "", "Input path from which to load events.");
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

  LOG_IF(FATAL, FLAGS_in.empty()) << "No input path provided.";
  LOG_IF(FATAL, FLAGS_out.empty()) << "No output file provided.";

  std::ofstream file;
  if (FLAGS_out != "stdout") {
    file.open(FLAGS_out);
  }
  std::ostream& out = FLAGS_out == "stdout" ? std::cout : file;
  LOG_IF(FATAL, !out) << "Unable to open output stream";

  LOG(INFO) << "Loading trace events...";

  tools::storage::Reader reader(FLAGS_in);
  for (auto& record : reader) {
    std::vector<uint8_t> buffer(record.size);
    std::memcpy(buffer.data(), record.src, record.size);
    TraceEvent event(std::move(buffer));
    out << event.toJson() << "\n";
  }

  LOG(INFO) << "Timeout.";

  return 0;
}

}  // namespace inspector

int main(int argc, char* argv[]) { return inspector::main(argc, argv); }