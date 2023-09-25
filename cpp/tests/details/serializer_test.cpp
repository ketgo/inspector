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

#include <gtest/gtest.h>

#include <vector>

#include <inspector/details/serializer.hpp>

using namespace inspector;

template <class T>
class SerializationStorageSizeTestFixture : public ::testing::Test {};

TYPED_TEST_SUITE_P(SerializationStorageSizeTestFixture);

TYPED_TEST_P(SerializationStorageSizeTestFixture, TestStorageSize) {
  TypeParam obj = {};
  ASSERT_EQ(details::storageSize(obj), sizeof(uint8_t) + sizeof(obj));
}

REGISTER_TYPED_TEST_SUITE_P(SerializationStorageSizeTestFixture,
                            TestStorageSize);

using TypeSetA =
    ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t, int16_t, int32_t,
                     int64_t, float, double, char>;
INSTANTIATE_TYPED_TEST_SUITE_P(StorageSize, SerializationStorageSizeTestFixture,
                               TypeSetA);

TEST(SerializationStorageSizeTestFixture, TestStorageSizeStringLiteral) {
  const char* obj = "testing-1";
  ASSERT_EQ(details::storageSize(obj),
            sizeof(uint8_t) + std::strlen(obj) + sizeof(char));
}

TEST(SerializationStorageSizeTestFixture, TestStorageSizeString) {
  const std::string obj = {"testing-1"};
  ASSERT_EQ(details::storageSize(obj),
            sizeof(uint8_t) + sizeof(std::size_t) + obj.size() * sizeof(char));
}

TEST(SerializationStorageSizeTestFixture, TestStorageSizeMultipleTypes) {
  const char* obj_0 = "testing-0";
  const std::string obj_1 = {"testing-1"};
  const char obj_2 = {};
  uint8_t obj_3 = {};
  ASSERT_EQ(details::storageSize(obj_0, obj_1, obj_2, obj_3),
            (sizeof(uint8_t) + std::strlen(obj_0) + sizeof(char)) +
                (sizeof(uint8_t) + sizeof(std::size_t) +
                 obj_1.size() * sizeof(char)) +
                (sizeof(uint8_t) + sizeof(obj_2)) +
                (sizeof(uint8_t) + sizeof(obj_3)));
}

template <class T>
class SerializationDumpLoadTestFixture : public ::testing::Test {
 public:
  SerializationDumpLoadTestFixture() : buffer_(10) {}

  static T value_;
  static DataType type_;
  std::vector<uint8_t> buffer_;
};

TYPED_TEST_SUITE_P(SerializationDumpLoadTestFixture);

TYPED_TEST_P(SerializationDumpLoadTestFixture, TestDump) {
  details::dump(this->buffer_.data(),
                SerializationDumpLoadTestFixture<TypeParam>::value_);
  ASSERT_EQ(SerializationDumpLoadTestFixture<TypeParam>::type_,
            details::dataType(this->buffer_.data()));
  ASSERT_EQ(SerializationDumpLoadTestFixture<TypeParam>::value_,
            details::load<TypeParam>(this->buffer_.data()));
  ASSERT_THROW(details::load<const char*>(this->buffer_.data()),
               std::runtime_error);
}

REGISTER_TYPED_TEST_SUITE_P(SerializationDumpLoadTestFixture, TestDump);

using TypeSetB =
    ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t, int16_t, int32_t,
                     int64_t, float, double, char, std::string>;
INSTANTIATE_TYPED_TEST_SUITE_P(DumpAndLoad, SerializationDumpLoadTestFixture,
                               TypeSetB);

template <>
uint8_t SerializationDumpLoadTestFixture<uint8_t>::value_ = 10;
template <>
DataType SerializationDumpLoadTestFixture<uint8_t>::type_ =
    DataType::TYPE_UINT8;

template <>
uint16_t SerializationDumpLoadTestFixture<uint16_t>::value_ = 11;
template <>
DataType SerializationDumpLoadTestFixture<uint16_t>::type_ =
    DataType::TYPE_UINT16;

template <>
uint32_t SerializationDumpLoadTestFixture<uint32_t>::value_ = 12;
template <>
DataType SerializationDumpLoadTestFixture<uint32_t>::type_ =
    DataType::TYPE_UINT32;

template <>
uint64_t SerializationDumpLoadTestFixture<uint64_t>::value_ = 13;
template <>
DataType SerializationDumpLoadTestFixture<uint64_t>::type_ =
    DataType::TYPE_UINT64;

template <>
int16_t SerializationDumpLoadTestFixture<int16_t>::value_ = -10;
template <>
DataType SerializationDumpLoadTestFixture<int16_t>::type_ =
    DataType::TYPE_INT16;

template <>
int32_t SerializationDumpLoadTestFixture<int32_t>::value_ = -11;
template <>
DataType SerializationDumpLoadTestFixture<int32_t>::type_ =
    DataType::TYPE_INT32;

template <>
int64_t SerializationDumpLoadTestFixture<int64_t>::value_ = -12;
template <>
DataType SerializationDumpLoadTestFixture<int64_t>::type_ =
    DataType::TYPE_INT64;

template <>
float SerializationDumpLoadTestFixture<float>::value_ = 10.0242;
template <>
DataType SerializationDumpLoadTestFixture<float>::type_ = DataType::TYPE_FLOAT;

template <>
double SerializationDumpLoadTestFixture<double>::value_ = 11.035215;
template <>
DataType SerializationDumpLoadTestFixture<double>::type_ =
    DataType::TYPE_DOUBLE;

template <>
char SerializationDumpLoadTestFixture<char>::value_ = 'a';
template <>
DataType SerializationDumpLoadTestFixture<char>::type_ = DataType::TYPE_CHAR;

template <>
std::string SerializationDumpLoadTestFixture<std::string>::value_ = "testing-0";
template <>
DataType SerializationDumpLoadTestFixture<std::string>::type_ =
    DataType::TYPE_STRING;

TEST(SerializationDumpLoadTestFixture, TestDumpAndLoadStringLiteral) {
  std::vector<uint8_t> buffer(10, 0);
  const char* value = "testing-1";

  details::dump(buffer.data(), value);
  ASSERT_EQ(DataType::TYPE_CSTRING, details::dataType(buffer.data()));
  ASSERT_EQ(std::strcmp(value, details::load<const char*>(buffer.data())), 0);
  ASSERT_THROW(details::load<std::string>(buffer.data()), std::runtime_error);
}