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

#include "tools/data/testing.hpp"

#include <boost/filesystem.hpp>
#include <iostream>

namespace inspector {
namespace tools {
namespace data {

namespace {

/**
 * @brief Construct a path to a temporary directory.
 *
 */
std::string tempPath() {
  auto path = utils::currentDir() / boost::filesystem::path("_tmp") /
              boost::filesystem::unique_path();
  return path.string();
}

}  // namespace

TestHarness::TestHarness(const bool remove) : temp_dir_(tempPath(), remove) {
  if (!remove) {
    std::cout << "Temporary Test Data Directoy: " << temp_dir_.path() << "\n";
  }
}

const utils::TempDir& TestHarness::tempDir() const { return temp_dir_; }

}  // namespace data
}  // namespace tools
}  // namespace inspector