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

#include "tools/viewers/perfetto/generator.hpp"

DEFINE_string(in, "", "Path to input directory.");
DEFINE_string(out, "", "Path to output file.");

namespace inspector {
namespace tools {

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  LOG_IF(FATAL, FLAGS_in.empty()) << "No input directory provided.";
  LOG_IF(FATAL, FLAGS_out.empty()) << "No output file provided.";

  generatePerfetto(FLAGS_in, FLAGS_out);

  return 0;
}

}  // namespace tools
}  // namespace inspector

int main(int argc, char* argv[]) { return inspector::tools::main(argc, argv); }