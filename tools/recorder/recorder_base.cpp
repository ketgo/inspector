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

#include "tools/recorder/recorder_base.hpp"

#include <glog/logging.h>

#include <thread>

namespace inspector {
namespace tools {

RecorderBase::RecorderBase(const std::string& name)
    : clock_(), name_(name), count_(0), stop_(false) {}

void RecorderBase::start(const std::chrono::microseconds duration) {
  while (!static_cast<bool>(stop_)) {
    LOG(INFO) << "[" << this->name_ << "]: Alive";
    const auto start_time = clock_.now();
    ++count_;
    this->record();
    const auto record_duration = clock_.now() - start_time;
    if (duration < record_duration) {
      LOG(WARNING) << "Recording " << count_
                   << " took long: " << record_duration.count() << " > "
                   << duration.count();
    } else {
      std::this_thread::sleep_for(duration - record_duration);
    }
  }
  // Performing last record operation to ensure all traces are recorded.
  this->record();
  LOG(INFO) << "[" << this->name_ << "]: Stopped";
}

bool RecorderBase::isAlive() const { return !stop_ && count_ > 0; }

void RecorderBase::stop() {
  LOG(INFO) << "[" << this->name_ << "]: Stopping...";
  stop_ = true;
}

const std::string& RecorderBase::name() const { return name_; }

uint64_t RecorderBase::recordCount() const { return count_; }

}  // namespace tools
}  // namespace inspector