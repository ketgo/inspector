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

#include "examples/cpp/number_generator.hpp"

#include <glog/logging.h>
#include <inspector/trace.hpp>

namespace inspector {
namespace examples {
namespace {

/**
 * @brief Check if a given number is prime.
 *
 */
static bool isPrime(int num) {
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

}  // namespace

PrimeNumberGenerator::PrimeNumberGenerator() : last_prime_(1) {}

void PrimeNumberGenerator::operator()() {
  inspector::SyncScope scope("PrimeNumberGenerator", last_prime_);

  while (!isPrime(++last_prime_))
    ;
  LOG(INFO) << "Next Prime: " << last_prime_;
}

}  // namespace examples
}  // namespace inspector