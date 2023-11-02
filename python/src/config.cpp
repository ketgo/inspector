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

#include <inspector/config.hpp>

namespace py = pybind11;

/**
 * @brief Binding the C++ config module to the given python module.
 *
 * @param m Reference to the python module.
 */
void bindConfig(py::module &m) {
  py::module config_m = m.def_submodule(
      "Config", "Configuration module for the inspector library.");

  config_m.def("event_queue_name", &inspector::Config::eventQueueName,
               "Get the name of the process shared event queue used by the "
               "inspector "
               "library to publish trace events for consumption by the trace "
               "reader.");
  config_m.def(
      "set_event_queue_name", &inspector::Config::setEventQueueName,
      "Set the name of the process shared event queue used by the "
      "inspector library to publish trace events for consumption by the "
      "trace reader.",
      py::arg("name"));
  config_m.def("is_trace_disabled", &inspector::Config::isTraceDisabled,
               "Check if tracing is disabled.");
  config_m.def("disable_trace", &inspector::Config::disableTrace,
               "Disable capturing of all trace events.");
  config_m.def("enable_trace", &inspector::Config::enableTrace,
               "Enable capturing of all trace events.");
}