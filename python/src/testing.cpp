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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <inspector/details/logging.hpp>
#include <inspector/details/trace_event.hpp>
#include <inspector/trace_event.hpp>

#include "cpp/tests/testing.hpp"

namespace py = pybind11;

/**
 * @brief Method to write a test log.
 *
 */
void writeTestLog() { LOG_ERROR << "Test Log Message"; }

/**
 * @brief Method to get a test trace event.
 *
 */
inspector::TraceEvent getTestTraceEvent() {
  constexpr const char *kEventName = "test-event";
  constexpr auto kType = 1;
  constexpr auto kCounter = 2;
  constexpr auto kPid = 1;
  constexpr auto kTid = 1;
  constexpr auto kTimestampNs = 1000;
  constexpr double kDebugArg1 = 3.5;
  constexpr int64_t kDebugArg2 = -1;
  constexpr const char *kDebugArg3 = "test-data";

  const size_t storage_size = inspector::details::traceEventStorageSize(
      kEventName, kDebugArg1, kDebugArg2, kDebugArg3);
  std::vector<uint8_t> buffer(storage_size);

  inspector::details::MutableTraceEvent mutable_event(buffer.data(),
                                                      buffer.size());
  mutable_event.setType(kType);
  mutable_event.setCounter(kCounter);
  mutable_event.setPid(kPid);
  mutable_event.setTid(kTid);
  mutable_event.setTimestampNs(kTimestampNs);
  mutable_event.appendDebugArgs(kEventName, kDebugArg1, kDebugArg2, kDebugArg3);

  return inspector::TraceEvent(std::move(buffer));
}

void bindTesting(py::module &m) {
  py::module testing_m =
      m.def_submodule("testing", "Utility methods for testing.");

  testing_m.def("write_test_log", &writeTestLog);
  testing_m.def("remove_event_queue", &inspector::testing::removeEventQueue);
  testing_m.def("empty_event_queue", &inspector::testing::emptyEventQueue);
  testing_m.def("get_test_trace_event", &getTestTraceEvent);
}