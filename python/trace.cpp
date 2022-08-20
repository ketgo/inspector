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

#include <inspector/trace.hpp>

namespace py = pybind11;

// -------------------------------
// Python Trace Event Wrappers
// -------------------------------

template <char TraceTag>
void PythonTraceEventNoArgs(const std::string& name) {
  if (inspector::details::Config::Get().disable_tracing) {
    return;
  }
  inspector::TraceEvent event(TraceTag, name);
  inspector::details::WriteTraceEvent(event);
}

template <char TraceTag>
void PythonTraceEvent(const std::string& name, const py::args& args,
                      const py::kwargs& kwargs) {
  if (inspector::details::Config::Get().disable_tracing) {
    return;
  }
  inspector::TraceEvent event(TraceTag, name);
  for (auto& arg : args) {
    event.SetArgs(py::str(arg));
  }
  for (auto& kwarg : kwargs) {
    event.SetArgs(inspector::MakeKwarg<std::string>(
        std::string(py::str(kwarg.first)).c_str(), py::str(kwarg.second)));
  }
  inspector::details::WriteTraceEvent(event);
}

// -------------------------------

void BindTrace(py::module& m) {
  m.def("sync_begin", &PythonTraceEvent<inspector::kSyncBeginTag>);
  m.def("sync_end", &PythonTraceEventNoArgs<inspector::kSyncEndTag>);
  m.def("async_begin", &PythonTraceEvent<inspector::kAsyncBeginTag>);
  m.def("async_instance", &PythonTraceEvent<inspector::kAsyncInstanceTag>);
  m.def("async_end", &PythonTraceEvent<inspector::kAsyncEndTag>);
  m.def("flow_begin", &PythonTraceEvent<inspector::kFlowBeginTag>);
  m.def("flow_instance", &PythonTraceEvent<inspector::kFlowInstanceTag>);
  m.def("flow_end", &PythonTraceEvent<inspector::kFLowEndTag>);
}