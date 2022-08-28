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

#include <glog/logging.h>

#include <thread>
#include <vector>

#include "examples/cpp/task.hpp"

namespace inspector {
namespace examples {

void PeriodicTask::Run(const size_t thread_pool_size) {
  LOG(INFO) << "Starting periodic task...";

  std::vector<std::thread> threads(thread_pool_size);
  for (auto& thread : threads) {
    thread = std::thread(boost::bind(&boost::asio::io_context::run, &io_));
  }
  for (auto& thread : threads) {
    thread.join();
  }
}

void PeriodicTask::Tick() {
  SyncScope __tick("PeriodicTask::Tick");

  task_();

  timer_.expires_at(timer_.expiry() + interval_ns_);
  AsyncWait(timer_, boost::bind(&PeriodicTask::Tick, this));
}

void PeriodicTask::Stop() {
  SyncScope __stop("PeriodicTask::Stop");

  io_.stop();
}

}  // namespace examples
}  // namespace inspector