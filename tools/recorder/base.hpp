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

#pragma once

#include <chrono>
#include <string>

namespace inspector {
namespace tools {

/**
 * @brief Method to initialize recorder tool.
 *
 * The method initializes tool by setting up signal handlers.
 */
void initialize();

/**
 * @brief The class `RecorderBase` is an abstract base class for implementing a
 * recorder. Its designed to perform unit of work recording periodically at a
 * fixed cadance.
 *
 */
class RecorderBase {
 public:
  virtual ~RecorderBase() = default;

  explicit RecorderBase(const std::string& name) : name_(name) {}

  /**
   * @brief Initialize the recorder.
   *
   */
  virtual void initialize();

  /**
   * @brief Recording task performed periodically.
   *
   */
  virtual void record() = 0;

  /**
   * @brief Method to start the recorder. Note that this is a blocking call.
   *
   */
  void run(const std::chrono::microseconds duration);

  /**
   * @brief Flag to check if recorder is terminating.
   *
   */
  bool isTerminating() const;

 protected:
  std::chrono::steady_clock clock_;

 private:
  const std::string name_;
};

}  // namespace tools
}  // namespace inspector