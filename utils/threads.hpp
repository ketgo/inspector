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

#include <thread>
#include <vector>

namespace utils {

/**
 * @brief Utility class to run multiple tasks in separate threads.
 *
 */
class Threads {
  using ThreadPool = std::vector<std::thread>;

 public:
  /**
   * @brief Type traits.
   *
   */
  using iterator = ThreadPool::iterator;

  /**
   * @brief Construct a new Threads object.
   *
   * @param n Number of threads to run.
   */
  Threads(const size_t n) : threads_(n) {}

  /**
   * @brief Destroy the Threads object.
   *
   * Calls the `wait` method.
   *
   */
  ~Threads() { Wait(); }

  /**
   * @brief Get iterator to the first thread in the thread pool managed by the
   * runner.
   */
  iterator begin() { return threads_.begin(); }

  /**
   * @brief Get iterator to one past the last thread in the thread pool managed
   * by the runner.
   */
  iterator end() { return threads_.end(); }

  /**
   * @brief Get reference to the thread at given offset.
   *
   */
  std::thread& operator[](const size_t idx) { return threads_[idx]; }

  /**
   * @brief Wait till all the tasks are completed.
   *
   */
  void Wait() {
    for (auto& thread : threads_) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

 private:
  ThreadPool threads_;
};

/**
 * @brief Utility method to run a callable in multiple threads.
 *
 * @tparam Function Type of callable.
 * @tparam Args Type of arguments passed to the callable.
 * @param N Number of threads to spawn.
 * @param func Rvalue reference to the callable.
 * @param args Rvalue reference to the arguments passed to the callable.
 */
template <class Function, class... Args>
void RunThreads(const size_t N, Function&& func, Args&&... args) {
  Threads threads(N);
  for (auto& thread : threads) {
    thread = std::thread(std::move(func), std::move(args)...);
  }
}

}  // namespace utils
