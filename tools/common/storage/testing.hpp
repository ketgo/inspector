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

#include "utils/tempdir.hpp"

/**
 * @brief Disable thread safety sanitization check.
 *
 */
#define NO_TSAN __attribute__((no_sanitize("thread")))

namespace inspector {
namespace tools {
namespace storage {

/**
 * @brief Test harness containing utility methods for unit testing. The harness
 * can be used by either deriving from it a test fixture or directly creating a
 * harness object in each unit test.
 *
 */
class TestHarness {
 public:
  /**
   * @brief Construct a new TestHarness object.
   *
   * As part of the CTOR a temporary directory will be created for use by a unit
   * test.
   *
   */
  explicit TestHarness(const bool remove = true);

  /**
   * @brief Get the temporary directory assigned to the unit test.
   *
   * @returns Constant reference to the temporary directory.
   */
  const utils::TempDir& tempDir() const;

 private:
  const utils::TempDir temp_dir_;
};

}  // namespace storage
}  // namespace tools
}  // namespace inspector