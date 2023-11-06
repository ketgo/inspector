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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <inspector/trace_event.hpp>
#include <inspector/trace_reader.hpp>

namespace py = pybind11;

/**
 * @brief Binding the C++ trace event module to the given python module.
 *
 * @param m Reference to the python module.
 */
void bindTraceEvent(py::module &m) {
  py::class_<inspector::DebugArg> debug_arg(m, "DebugArg");

  py::enum_<inspector::DebugArg::Type>(debug_arg, "Type")
      .value("TYPE_INT16", inspector::DebugArg::Type::TYPE_INT16)
      .value("TYPE_INT32", inspector::DebugArg::Type::TYPE_INT32)
      .value("TYPE_INT64", inspector::DebugArg::Type::TYPE_INT64)
      .value("TYPE_UINT8", inspector::DebugArg::Type::TYPE_UINT8)
      .value("TYPE_UINT16", inspector::DebugArg::Type::TYPE_UINT16)
      .value("TYPE_UINT32", inspector::DebugArg::Type::TYPE_UINT32)
      .value("TYPE_UINT64", inspector::DebugArg::Type::TYPE_UINT64)
      .value("TYPE_FLOAT", inspector::DebugArg::Type::TYPE_FLOAT)
      .value("TYPE_DOUBLE", inspector::DebugArg::Type::TYPE_DOUBLE)
      .value("TYPE_CHAR", inspector::DebugArg::Type::TYPE_CHAR)
      .value("TYPE_STRING", inspector::DebugArg::Type::TYPE_STRING)
      .export_values();

  debug_arg
      .def("type", &inspector::DebugArg::type,
           "Get the type of the debug argument.")
      .def(
          "value",
          [](const inspector::DebugArg &self) {
            switch (self.type()) {
              case inspector::DebugArg::Type::TYPE_INT16: {
                return py::cast(self.value<int16_t>());
              }
              case inspector::DebugArg::Type::TYPE_INT32: {
                return py::cast(self.value<int32_t>());
              }
              case inspector::DebugArg::Type::TYPE_INT64: {
                return py::cast(self.value<int64_t>());
              }
              case inspector::DebugArg::Type::TYPE_UINT8: {
                return py::cast(self.value<uint8_t>());
              }
              case inspector::DebugArg::Type::TYPE_UINT16: {
                return py::cast(self.value<uint16_t>());
              }
              case inspector::DebugArg::Type::TYPE_UINT32: {
                return py::cast(self.value<uint32_t>());
              }
              case inspector::DebugArg::Type::TYPE_UINT64: {
                return py::cast(self.value<uint64_t>());
              }
              case inspector::DebugArg::Type::TYPE_FLOAT: {
                return py::cast(self.value<float>());
              }
              case inspector::DebugArg::Type::TYPE_DOUBLE: {
                return py::cast(self.value<double>());
              }
              case inspector::DebugArg::Type::TYPE_CHAR: {
                return py::cast(self.value<char>());
              }
              case inspector::DebugArg::Type::TYPE_STRING: {
                return py::cast(self.value<std::string>());
              }
            }

            throw std::runtime_error("Invalid debug argument type observed.");
          },
          "Get the value of the debug argument.");

  py::class_<inspector::DebugArgs>(m, "DebugArgs")
      .def("size", &inspector::DebugArgs::size,
           "Get the number of debug arguments.")
      .def("__len__", &inspector::DebugArgs::size)
      .def(
          "__iter__",
          [](const inspector::DebugArgs &self) {
            return py::make_iterator(self.begin(), self.end());
          },
          py::keep_alive<0, 1>());

  py::class_<inspector::TraceEvent>(m, "TraceEvent")
      .def("type", &inspector::TraceEvent::type, "Get the type of trace event.")
      .def("counter", &inspector::TraceEvent::counter,
           "Get the trace event counter.")
      .def("timestamp_ns", &inspector::TraceEvent::timestampNs,
           "Get the timestamp in nanoseconds of the trace event.")
      .def("pid", &inspector::TraceEvent::pid, "Get the process identifier.")
      .def("tid", &inspector::TraceEvent::tid, "Get the thread identifier.")
      .def(
          "name",
          [](const inspector::TraceEvent &self) {
            return std::string(self.name());
          },
          "Get the name of trace event.")
      .def("debug_args", &inspector::TraceEvent::debugArgs,
           "Get the debug arguments which are part of the trace event.");

  m.def(
      "read_trace_event", []() { return inspector::readTraceEvent(); },
      "Read a stored trace event from the process shared queue.");
  m.def(
      "read_trace_event",
      [](const std::chrono::microseconds timeout) {
        return inspector::readTraceEvent(timeout);
      },
      "Read a stored trace event from the process shared queue.");
}