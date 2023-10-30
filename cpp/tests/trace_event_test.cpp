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

#include <inspector/trace_event.hpp>
#include <inspector/details/trace_event.hpp>

using namespace inspector;

template <class T>
class TraceEventTestFixture : public ::testing::Test {
 public:
 public:
  TraceEventTestFixture() : buffer_() {}

  static T value_;
  static DebugArg::Type type_;
  std::vector<uint8_t> buffer_;
};

TYPED_TEST_SUITE_P(TraceEventTestFixture);

TYPED_TEST_P(TraceEventTestFixture, TestMutableAndNonMutableTraceEvent) {
  constexpr const char* kEventName = "test-event";
  constexpr auto kType = 1;
  constexpr auto kCounter = 2;
  constexpr auto kPid = 1;
  constexpr auto kTid = 1;
  constexpr auto kTimestampNs = 1000;

  this->buffer_.resize(details::traceEventStorageSize(
      kEventName, TraceEventTestFixture<TypeParam>::value_));

  details::MutableTraceEvent mutable_event(this->buffer_.data(),
                                           this->buffer_.size());
  mutable_event.setType(kType);
  mutable_event.setCounter(kCounter);
  mutable_event.setPid(kPid);
  mutable_event.setTid(kTid);
  mutable_event.setTimestampNs(kTimestampNs);
  mutable_event.appendDebugArgs(kEventName,
                                TraceEventTestFixture<TypeParam>::value_);

  TraceEvent event(std::move(this->buffer_));
  ASSERT_EQ(event.type(), kType);
  ASSERT_EQ(event.counter(), kCounter);
  ASSERT_EQ(event.pid(), kPid);
  ASSERT_EQ(event.tid(), kTid);
  ASSERT_EQ(event.timestampNs(), kTimestampNs);
  ASSERT_EQ(std::strcmp(event.name(), kEventName), 0);
  ASSERT_EQ(event.debugArgs().size(), 1);
  auto it = event.debugArgs().begin();
  ASSERT_EQ(it->type(), TraceEventTestFixture<TypeParam>::type_);
  ASSERT_EQ(it->value<TypeParam>(), TraceEventTestFixture<TypeParam>::value_);
  ASSERT_THROW(it->value<const char*>(), std::runtime_error);
}

REGISTER_TYPED_TEST_SUITE_P(TraceEventTestFixture,
                            TestMutableAndNonMutableTraceEvent);

using TypeSet = ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t, int16_t,
                                 int32_t, int64_t, float, double, char>;
INSTANTIATE_TYPED_TEST_SUITE_P(MyTest, TraceEventTestFixture, TypeSet, );

template <>
uint8_t TraceEventTestFixture<uint8_t>::value_ = 10;
template <>
DebugArg::Type TraceEventTestFixture<uint8_t>::type_ =
    DebugArg::Type::TYPE_UINT8;

template <>
uint16_t TraceEventTestFixture<uint16_t>::value_ = 11;
template <>
DebugArg::Type TraceEventTestFixture<uint16_t>::type_ =
    DebugArg::Type::TYPE_UINT16;

template <>
uint32_t TraceEventTestFixture<uint32_t>::value_ = 12;
template <>
DebugArg::Type TraceEventTestFixture<uint32_t>::type_ =
    DebugArg::Type::TYPE_UINT32;

template <>
uint64_t TraceEventTestFixture<uint64_t>::value_ = 13;
template <>
DebugArg::Type TraceEventTestFixture<uint64_t>::type_ =
    DebugArg::Type::TYPE_UINT64;

template <>
int16_t TraceEventTestFixture<int16_t>::value_ = -10;
template <>
DebugArg::Type TraceEventTestFixture<int16_t>::type_ =
    DebugArg::Type::TYPE_INT16;

template <>
int32_t TraceEventTestFixture<int32_t>::value_ = -11;
template <>
DebugArg::Type TraceEventTestFixture<int32_t>::type_ =
    DebugArg::Type::TYPE_INT32;

template <>
int64_t TraceEventTestFixture<int64_t>::value_ = -12;
template <>
DebugArg::Type TraceEventTestFixture<int64_t>::type_ =
    DebugArg::Type::TYPE_INT64;

template <>
float TraceEventTestFixture<float>::value_ = 10.0242;
template <>
DebugArg::Type TraceEventTestFixture<float>::type_ = DebugArg::Type::TYPE_FLOAT;

template <>
double TraceEventTestFixture<double>::value_ = 11.035215;
template <>
DebugArg::Type TraceEventTestFixture<double>::type_ =
    DebugArg::Type::TYPE_DOUBLE;

template <>
char TraceEventTestFixture<char>::value_ = 'a';
template <>
DebugArg::Type TraceEventTestFixture<char>::type_ = DebugArg::Type::TYPE_CHAR;

TEST(TraceEventTestFixture,
     TestMutableAndNonMutableTraceEventWithStringDebugArg) {
  constexpr const char* kEventName = "test-event";
  constexpr auto kType = 1;
  constexpr auto kCounter = 2;
  constexpr auto kPid = 1;
  constexpr auto kTid = 1;
  constexpr auto kTimestampNs = 1000;
  const std::string kValue = "testing-0";

  std::vector<uint8_t> buffer(
      details::traceEventStorageSize(kEventName, kValue));

  details::MutableTraceEvent mutable_event(buffer.data(), buffer.size());
  mutable_event.setType(kType);
  mutable_event.setCounter(kCounter);
  mutable_event.setPid(kPid);
  mutable_event.setTid(kTid);
  mutable_event.setTimestampNs(kTimestampNs);
  mutable_event.appendDebugArgs(kEventName, kValue);

  TraceEvent event(std::move(buffer));
  ASSERT_EQ(event.type(), kType);
  ASSERT_EQ(event.counter(), kCounter);
  ASSERT_EQ(event.pid(), kPid);
  ASSERT_EQ(event.tid(), kTid);
  ASSERT_EQ(event.timestampNs(), kTimestampNs);
  ASSERT_EQ(std::strcmp(event.name(), kEventName), 0);
  ASSERT_EQ(event.debugArgs().size(), 1);
  auto it = event.debugArgs().begin();
  ASSERT_EQ(it->type(), DebugArg::Type::TYPE_STRING);
  ASSERT_EQ(it->value<std::string>(), kValue);
  ASSERT_THROW(it->value<uint8_t>(), std::runtime_error);
}