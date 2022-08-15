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

#include <inspector/tracer.hpp>

namespace py = pybind11;

// -------------------------------
// Synchronous Scope Trace Events
// -------------------------------

void PythonSyncBegin(const std::string& name, const py::args& args,
                     const py::kwargs& kwargs) {
  inspector::details::TraceEvent event(inspector::kSyncBeginTag, name);
  // Adding passed arguments to trace
  for (auto& arg : args) {
    event.SetArgs(py::str(arg));
  }
  // Adding keyword argument to trace
  for (auto& kwarg : kwargs) {
    event.SetKwargs(inspector::MakeKwarg<std::string>(
        std::string(py::str(kwarg.first)).c_str(), py::str(kwarg.second)));
  }
  inspector::details::TraceWriter().Write(event.String());
}

void BindTracer(py::module& m) {
  m.def("sync_begin", &PythonSyncBegin);
  m.def("sync_end", [](const std::string& name) {
    inspector::details::TraceEvent event(inspector::kSyncEndTag, name);
    inspector::details::TraceWriter().Write(event.String());
  });

  // -------------------------------
  // Asynchronous Scope Trace Events
  // -------------------------------

  m.def("async_begin", [](const std::string& name, const py::args& args,
                          const py::kwargs& kwargs) {
    inspector::details::TraceEvent event(inspector::kAsyncInstanceTag, name);
    // Adding passed arguments to trace
    for (auto& arg : args) {
      event.SetArgs(py::str(arg));
    }
    // Adding keyword argument to trace
    for (auto& kwarg : kwargs) {
      event.SetKwargs(inspector::MakeKwarg<std::string>(
          std::string(py::str(kwarg.first)).c_str(), py::str(kwarg.second)));
    }
    inspector::details::TraceWriter().Write(event.String());
  });

  m.def("async_instance", [](const std::string& name, const py::args& args,
                             const py::kwargs& kwargs) {
    inspector::details::TraceEvent event(inspector::kAsyncInstanceTag, name);
    // Adding passed arguments to trace
    for (auto& arg : args) {
      event.SetArgs(py::str(arg));
    }
    // Adding keyword argument to trace
    for (auto& kwarg : kwargs) {
      event.SetKwargs(inspector::MakeKwarg<std::string>(
          std::string(py::str(kwarg.first)).c_str(), py::str(kwarg.second)));
    }
    inspector::details::TraceWriter().Write(event.String());
  });

  m.def("async_end", [](const std::string& name, const py::args& args,
                        const py::kwargs& kwargs) {
    inspector::details::TraceEvent event(inspector::kAsyncEndTag, name);
    // Adding passed arguments to trace
    for (auto& arg : args) {
      event.SetArgs(py::str(arg));
    }
    // Adding keyword argument to trace
    for (auto& kwarg : kwargs) {
      event.SetKwargs(inspector::MakeKwarg<std::string>(
          std::string(py::str(kwarg.first)).c_str(), py::str(kwarg.second)));
    }
    inspector::details::TraceWriter().Write(event.String());
  });

  // -------------------------------
  // Flow Scope Trace Events
  // -------------------------------
}