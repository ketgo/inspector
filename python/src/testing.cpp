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
#include "cpp/tests/testing.hpp"

namespace py = pybind11;

/**
 * @brief Method to write a test log.
 *
 */
void writeTestLog() { LOG_ERROR << "Test Log Message"; }

void bindTesting(py::module& m) {
  py::module testing_m =
      m.def_submodule("testing", "Utility methods for testing.");

  testing_m.def("write_test_log", &writeTestLog);
  testing_m.def("remove_event_queue", &inspector::testing::removeEventQueue);
  testing_m.def("empty_event_queue", &inspector::testing::emptyEventQueue);
}