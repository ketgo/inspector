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

#include "utils/threads.hpp"

namespace utils {

Threads::Threads(const size_t n) : threads_(n) {}

Threads::~Threads() { Wait(); }

Threads::iterator Threads::begin() { return threads_.begin(); }

Threads::iterator Threads::end() { return threads_.end(); }

std::thread& Threads::operator[](const size_t idx) { return threads_[idx]; }

void Threads::Wait() {
  for (auto& thread : threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

}  // namespace utils