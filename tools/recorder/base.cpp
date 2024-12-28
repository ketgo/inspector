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

#include "tools/recorder/base.hpp"

#include <csignal>
#include <thread>

//
#include <glog/logging.h>

namespace inspector {
namespace tools {
namespace {

volatile std::sig_atomic_t __stop = 0;

void signalHandler(int signal) { __stop = signal; }

}  // namespace

void initialize() {
  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);
}

void RecorderBase::initialize() {}

void RecorderBase::run(const std::chrono::microseconds duration) {
  while (!static_cast<bool>(__stop)) {
    LOG(INFO) << "[" << this->name_ << "]: Alive";
    const auto start_time = clock_.now();
    this->record();
    const auto record_duration = clock_.now() - start_time;
    if (duration < record_duration) {
      LOG(WARNING) << "Recording took long: " << record_duration.count()
                   << " > " << duration.count();
    } else {
      std::this_thread::sleep_for(duration - record_duration);
    }
  }
  LOG(INFO) << "[" << this->name_ << "]: Stopped";
}

bool RecorderBase::isTerminating() const { return __stop; }

}  // namespace tools
}  // namespace inspector