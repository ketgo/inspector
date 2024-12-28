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

#include "tools/recorder/recorder.hpp"

#include <chrono>
#include <memory>
#include <thread>
#include <vector>

#include "tools/recorder/trace_recorder.hpp"

namespace inspector {
namespace tools {
namespace {

/**
 * @brief Tick interval of the monitor.
 *
 */
constexpr std::chrono::microseconds kTickIntervalUs{99'000};  // 99ms

class Manager {
 public:
  static Manager& instance() {
    static Manager mng;
    return mng;
  }

  void start(const std::string& out, const bool block) {
    if (!recorders_.empty()) {
      return;
    }
    recorders_.emplace_back(std::make_shared<TraceRecorder>(out));
    threads_.emplace_back(&RecorderBase::start, recorders_.back().get(),
                          kTickIntervalUs);
    if (block) {
      wait();
    }
  }

  void stop(const bool block) {
    for (auto recorder : recorders_) {
      recorder->stop();
    }
    if (block) {
      wait();
    }
  }

 private:
  void wait() {
    for (auto& thread : threads_) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  std::vector<std::shared_ptr<RecorderBase>> recorders_;
  std::vector<std::thread> threads_;
};

}  // namespace

void startRecorder(const std::string& out, const bool block) {
  Manager::instance().start(out, block);
}

void stopRecorder(const bool block) { Manager::instance().stop(block); }

}  // namespace tools
}  // namespace inspector