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

#include <random>

namespace utils {

/**
 * @brief Random number generator.
 *
 * @tparam T The type of number.
 */
template <class T>
class RandomNumberGenerator {
 public:
  /**
   * @brief Construct a new RandomNumberGenerator object.
   *
   * @param min Minimum possible value.
   * @param max Maximum possible value.
   */
  RandomNumberGenerator(const T min, const T max);

  /**
   * @brief Generate random number.
   *
   */
  T operator()();

 private:
  std::mt19937_64 eng_;
  std::uniform_int_distribution<T> dist_;
};

// ------------------------------------
// RandomNumberGenerator Implementation
// ------------------------------------

template <class T>
RandomNumberGenerator<T>::RandomNumberGenerator(const T min, const T max)
    : eng_(std::random_device{}()), dist_(min, max) {}

template <class T>
T RandomNumberGenerator<T>::operator()() {
  return T{dist_(eng_)};
}

// ------------------------------------

}  // namespace utils
