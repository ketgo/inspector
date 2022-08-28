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

#pragma once

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/function.hpp>

#include <inspector/trace.hpp>

namespace inspector {
namespace examples {

template <class AsyncWaitable, class Handler>
void AsyncWait(AsyncWaitable& waitable, Handler&& handler) {
  AsyncBegin("boost::async_wait");
  auto completion_handler =
      [=](const boost::system::error_code& error_code) -> void {
    handler();
    AsyncEnd("boost::async_wait");
  };
  waitable.async_wait(completion_handler);
}

/**
 * @brief The class `PeriodicTask` represents a task triggered periodically at a
 * given interval.
 *
 */
class PeriodicTask {
 public:
  /**
   * @brief Construct a new Periodic Task object,
   *
   * @tparam Task Type of task called periodically.
   * @tparam Args Type of arguments passed to the periodic task.
   * @param interval_ns Interval in nano seconds between each call to the task.
   * @param func Rvalue reference to the task.
   * @param args Rvalue reference to the task arguments.
   */
  template <class Task, class... Args>
  PeriodicTask(const uint64_t interval_ns, Task&& task, Args&&... args);

  /**
   * @brief Run the periodic task. This method is a blocking call.
   *
   * @param thread_pool_size Size of the thread pool.
   */
  void Run(const size_t thread_pool_size = 1);

 private:
  /**
   * @brief Tick method is called to run the user provided task periodically.
   *
   */
  void Tick();

  /**
   * @brief Stop executing the periodic task.
   *
   */
  void Stop();

  boost::asio::io_context io_;
  boost::asio::strand<boost::asio::io_context::executor_type> strand_;
  boost::asio::chrono::nanoseconds interval_ns_;
  boost::asio::steady_timer timer_;
  boost::function<void()> task_;
  boost::asio::signal_set signals_;
};

// -------------------------------------
// PeriodicTask Implementation
// -------------------------------------

template <class Task, class... Args>
PeriodicTask::PeriodicTask(const uint64_t interval_ns, Task&& task,
                           Args&&... args)
    : strand_(boost::asio::make_strand(io_)),
      interval_ns_(interval_ns),
      timer_(strand_, interval_ns_),
      task_(boost::bind(std::forward<Task>(task), std::forward<Args>(args)...)),
      signals_(io_) {
  // Registering signal handlers to gracefully terminate the periodic task
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif  // defined(SIGQUIT)
  signals_.async_wait(boost::bind(&PeriodicTask::Stop, this));

  // Add task to work queue
  AsyncWait(timer_, boost::bind(&PeriodicTask::Tick, this));
}

// -------------------------------------

}  // namespace examples
}  // namespace inspector