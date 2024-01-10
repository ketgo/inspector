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

template <inspector::EventType T>
void pythonTraceEvent(const std::string &name, const py::args &args) {
  if (inspector::Config::isTraceDisabled()) {
    return;
  }

  size_t storage_size = inspector::details::traceEventStorageSize() +
                        inspector::details::debugArgsStorageSize(name);
  for (auto &arg : args) {
    if (py::isinstance<py::str>(arg)) {
      storage_size +=
          inspector::details::debugArgStorageSize(py::cast<std::string>(arg));
    } else if (py::isinstance<py::int_>(arg)) {
      storage_size +=
          inspector::details::debugArgStorageSize(py::cast<int64_t>(arg));
    } else if (py::isinstance<py::float_>(arg)) {
      storage_size +=
          inspector::details::debugArgStorageSize(py::cast<double>(arg));
    } else {
      throw std::runtime_error(
          "Argument type not supported as debug argument.");
    }
  }

  std::vector<uint8_t> buffer(storage_size);
  auto event =
      inspector::details::MutableTraceEvent(buffer.data(), buffer.size());
  event.setType(static_cast<const inspector::event_type_t>(T));
  event.setCounter(++inspector::details::threadLocalCounter());
  event.setTimestampNs(
      std::chrono::system_clock::now().time_since_epoch().count());
  event.setPid(inspector::details::getPID());
  event.setTid(inspector::details::getTID());
  event.appendDebugArg(name);
  for (auto &arg : args) {
    if (py::isinstance<py::str>(arg)) {
      event.appendDebugArg(py::cast<std::string>(arg));
    } else if (py::isinstance<py::int_>(arg)) {
      event.appendDebugArg(py::cast<int64_t>(arg));
    } else if (py::isinstance<py::float_>(arg)) {
      event.appendDebugArg(py::cast<double>(arg));
    } else {
      // This block will never be called as the below error is already raised
      // above when the storage size is computed. It is still however left for
      // readability.
      throw std::runtime_error(
          "Argument type not supported as debug argument.");
    }
  }

  inspector::details::eventQueue().publish(buffer);
}

void pythonCounterEvent(const std::string &name, const py::object &arg) {
  if (py::isinstance<py::int_>(arg)) {
    inspector::counter(name.c_str(), py::cast<int64_t>(arg));
  } else if (py::isinstance<py::float_>(arg)) {
    inspector::counter(name.c_str(), py::cast<double>(arg));
  } else {
    throw std::runtime_error("Argument type not supported as debug argument.");
  }
}

// -------------------------------

void bindTrace(py::module &m) {
  py::enum_<inspector::EventType>(m, "EventType")
      .value("kSyncBeginTag", inspector::EventType::kSyncBeginTag)
      .value("kSyncEndTag", inspector::EventType::kSyncEndTag)
      .value("kAsyncBeginTag", inspector::EventType::kAsyncBeginTag)
      .value("kAsyncInstanceTag", inspector::EventType::kAsyncInstanceTag)
      .value("kAsyncEndTag", inspector::EventType::kAsyncEndTag)
      .value("kFlowBeginTag", inspector::EventType::kFlowBeginTag)
      .value("kFlowInstanceTag", inspector::EventType::kFlowInstanceTag)
      .value("kFlowEndTag", inspector::EventType::kFlowEndTag)
      .value("kCounterTag", inspector::EventType::kCounterTag);

  m.def("sync_begin", &pythonTraceEvent<inspector::EventType::kSyncBeginTag>);
  m.def("sync_end",
        [](const std::string &name) { inspector::syncEnd(name.c_str()); });
  m.def("async_begin", &pythonTraceEvent<inspector::EventType::kAsyncBeginTag>);
  m.def("async_instance",
        &pythonTraceEvent<inspector::EventType::kAsyncInstanceTag>);
  m.def("async_end", &pythonTraceEvent<inspector::EventType::kAsyncEndTag>);
  m.def("flow_begin", &pythonTraceEvent<inspector::EventType::kFlowBeginTag>);
  m.def("flow_instance",
        &pythonTraceEvent<inspector::EventType::kFlowInstanceTag>);
  m.def("flow_end", &pythonTraceEvent<inspector::EventType::kFlowEndTag>);
  m.def("counter", &pythonCounterEvent);
}