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

#include "examples/cpp/periodic_task.hpp"

#include <thread>
#include <vector>

#include <boost/bind/bind.hpp>
#include <glog/logging.h>
#include <inspector/trace.hpp>

namespace inspector {
namespace examples {
namespace {

/**
 * @brief Wrapper to publish trace events for `boost::async_wait` method.
 *
 * @tparam AsyncWaitable Any type which implements the `async_wait` method.
 * @tparam CompletionHandler Completion handler type.
 * @param waitable Reference to the waitable object.
 * @param handler Rvalue reference to the completion handler.
 */
template <class AsyncWaitable, class CompletionHandler>
void asyncWait(AsyncWaitable& waitable, CompletionHandler&& handler) {
  inspector::asyncBegin("boost::async_wait");
  auto completion_handler =
      [=](const boost::system::error_code& error_code) -> void {
    handler();
    inspector::asyncEnd("boost::async_wait");
  };
  waitable.async_wait(completion_handler);
}

}  // namespace

PeriodicTask::PeriodicTask(const uint64_t interval_ns, Task&& task)
    : strand_(boost::asio::make_strand(io_)),
      interval_ns_(interval_ns),
      timer_(strand_, interval_ns_),
      signals_(io_),
      task_(std::forward<Task>(task)) {
  // Registering signal handlers to gracefully terminate the periodic task
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif  // defined(SIGQUIT)
  signals_.async_wait(boost::bind(&PeriodicTask::stop, this));

  // Add task to work queue
  asyncWait(timer_, boost::bind(&PeriodicTask::tick, this));
}

void PeriodicTask::run(const size_t thread_pool_size) {
  LOG(INFO) << "Starting periodic task...";

  std::vector<std::thread> threads(thread_pool_size);
  for (auto& thread : threads) {
    thread = std::thread(boost::bind(&boost::asio::io_context::run, &io_));
  }
  for (auto& thread : threads) {
    thread.join();
  }
}

void PeriodicTask::tick() {
  inspector::SyncScope scope("PeriodicTask::tick");

  task_();

  timer_.expires_at(timer_.expiry() + interval_ns_);
  asyncWait(timer_, boost::bind(&PeriodicTask::tick, this));
}

void PeriodicTask::stop() {
  inspector::SyncScope scope("PeriodicTask::stop");

  io_.stop();
}

}  // namespace examples
}  // namespace inspector