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
#include <inspector/trace.hpp>

#include "examples/cpp/init.hpp"
#include "examples/cpp/periodic_task.hpp"

/**
 * @brief Generator for prime numbers.
 *
 */
class PrimeNumberGenerator {
 public:
  /**
   * @brief Construct a new Prime Number Generator object.
   *
   */
  PrimeNumberGenerator() : last_prime_(1) {}

  /**
   * @brief Generates next prime number
   *
   */
  void operator()() {
    TRACE_SCOPE("PrimeNumberGenerator");

    while (!IsPrime(++last_prime_))
      ;
    LOG(INFO) << "Next Prime: " << last_prime_;
  }

 private:
  /**
   * @brief Check if a given number is prime.
   *
   */
  static bool IsPrime(int num) {
    if (num == 1) {
      return false;
    }
    int i = 2;
    while (i * i <= num) {
      if (num % i == 0) {
        return false;
      }
      ++i;
    }
    return true;
  }

  int last_prime_;
};

int main(int argc, char* argv[]) {
  // Initializing glog logger
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);

  // Initialize inspector
  inspector::examples::InitInspector();

  LOG(INFO) << "Starting Trace Generator...";
  PrimeNumberGenerator generator;
  inspector::examples::PeriodicTask task(1000000000UL, generator);
  task.Run();

  return 0;
}
