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

#include "tools/common/storage/file_io.hpp"

#include <gtest/gtest.h>

#include "tools/common/storage/testing.hpp"

using namespace inspector::tools::storage;

namespace {

constexpr auto kTestDataFile = "tools/common/storage/file_io_test.data";
constexpr auto kTestDataFileSize = 7;  // bytes

}  // namespace

class FileIOTestFixture : public TestHarness, public ::testing::Test {
 protected:
  static constexpr auto kTestFile = "file_io_test.data";

  void SetUp() override { tempDir().copyFile(kTestDataFile, kTestFile); }
};

TEST_F(FileIOTestFixture, TestExists) {
  ASSERT_FALSE(File::exists("no_exist.data", tempDir().path()));
  ASSERT_TRUE(File::exists(kTestFile, tempDir().path()));
}

TEST_F(FileIOTestFixture, TestCtorAndRemove) {
  auto file = File("temp_test.data", tempDir().path());
  // Asserts that file exists
  ASSERT_TRUE(tempDir().fileExists("temp_test.data"));

  file.remove();
  // Asserts that file does not exist
  ASSERT_FALSE(tempDir().fileExists("temp_test.data"));
}

TEST_F(FileIOTestFixture, TestReadAndWrite) {
  auto file = File(kTestFile, tempDir().path());

  ASSERT_EQ(file.size(), kTestDataFileSize);

  std::string data(kTestDataFileSize, ' ');
  file.read(const_cast<char*>(data.data()), data.size(), 0);
  ASSERT_EQ(data, "testing");

  const std::string write_data = "testing";
  file.write(write_data.data(), write_data.size(), kTestDataFileSize);
  ASSERT_EQ(file.size(), 2 * kTestDataFileSize);
  ASSERT_EQ(tempDir().readFile(kTestFile), "testingtesting");
}

TEST_F(FileIOTestFixture, TestResize) {
  auto file = File(kTestFile, tempDir().path());

  ASSERT_EQ(file.size(), kTestDataFileSize);
  file.resize(10);
  ASSERT_EQ(file.size(), 10);
}