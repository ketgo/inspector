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

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <stdio.h>

#include <chrono>
#include <csignal>
#include <inspector/trace_reader.hpp>

#include "tools/common/storage/storage.hpp"

DEFINE_string(out, "", "Path to storage directory.");

/**
 * @brief Block size in bytes to use for storage.
 *
 */
constexpr auto kBlockSize = 1024UL * 1024UL * 100UL;  // 100MB

/**
 * @brief Tick interval of the monitor.
 *
 */
constexpr auto kTickIntervalUs = 99'000UL;  // 99ms

namespace inspector {

volatile static std::sig_atomic_t __signal = 0;

void signal_handler(int signal) { __signal = signal; }

void tick(tools::storage::Writer& writer) {
  while (true) {
    auto event = readTraceEvent();
    if (event.isEmpty()) {
      break;
    }
    const auto span = event.span();
    writer.write({event.timestampNs(), span.first, span.second});
  }
}

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG_IF(FATAL, FLAGS_out.empty()) << "No output file provided.";

  std::signal(SIGINT, signal_handler);

  tools::storage::Writer writer(FLAGS_out, kBlockSize);
  LOG(INFO) << "Starting recorder...";
  LOG(INFO) << "Recorder started.";
  // Print to stdout so that the wrapper can know when recording has started.
  ::printf("Recorder started.\n");
  ::fflush(stdout);
  while (!__signal) {
    const auto start_time = std::chrono::system_clock::now();
    tick(writer);
    const unsigned long duration_us =
        (std::chrono::system_clock::now() - start_time).count() / 1000UL;
    if (kTickIntervalUs < duration_us) {
      LOG(WARNING) << "Tick ran long: " << duration_us << " > "
                   << kTickIntervalUs;
    } else {
      ::usleep(kTickIntervalUs - duration_us);
    }
  }
  LOG(INFO) << "Stopping recorder...";
  LOG(INFO) << "Recorder stopped.";

  ::printf("Output: %s\n", FLAGS_out.c_str());
  ::fflush(stdout);

  return 0;
}

}  // namespace inspector

int main(int argc, char* argv[]) { return inspector::main(argc, argv); }