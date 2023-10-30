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
void pythonTraceEventWithoutArgs(const std::string& name) {
  inspector::details::writeTraceEvent(static_cast<inspector::event_type_t>(T),
                                      name.c_str());
}

template <inspector::EventType T>
void pythonTraceEventWithArgs(const std::string& name, const py::args& args) {
  if (inspector::Config::isTraceDisabled()) {
    return;
  }

  size_t storage_size = inspector::details::traceEventStorageSize();
  for (auto& arg : args) {
    if (py::isinstance<py::str>(arg)) {
      storage_size +=
          inspector::details::debugArgStorageSize(std::string(py::str(arg)));
    } else if (py::isinstance<py::str>(arg)) {
    }
  }

  auto result =
      inspector::eventQueue().tryPublish(traceEventStorageSize(name, args...));
  if (result.first != bigcat::CircularQueue::Status::OK) {
    return;
  }
  auto event = MutableTraceEvent(result.second.data(), result.second.size());
  event.setType(type);
  event.setCounter(++threadLocalCounter());
  event.setTimestampNs(
      std::chrono::system_clock::now().time_since_epoch().count());
  event.setPid(getPID());
  event.setTid(getTID());
  event.appendDebugArg(name);
  for (auto& arg : args) {
    event.appendDebugArg(py::str(arg));
  }
}

// -------------------------------

void bindTrace(py::module& m) {
  m.def("sync_begin",
        &pythonTraceEventWithoutArgs<inspector::EventType::kSyncBeginTag>);
  m.def("sync_end",
        &pythonTraceEventWithoutArgs<inspector::EventType::kSyncEndTag>);
  m.def("async_begin",
        &pythonTraceEventWithoutArgs<inspector::EventType::kAsyncBeginTag>);
  m.def("async_instance",
        &pythonTraceEventWithoutArgs<inspector::EventType::kAsyncInstanceTag>);
  m.def("async_end",
        &pythonTraceEventWithoutArgs<inspector::EventType::kAsyncEndTag>);
  m.def("flow_begin",
        &pythonTraceEventWithoutArgs<inspector::EventType::kFlowBeginTag>);
  m.def("flow_instance",
        &pythonTraceEventWithoutArgs<inspector::EventType::kFlowInstanceTag>);
  m.def("flow_end",
        &pythonTraceEventWithoutArgs<inspector::EventType::kFlowEndTag>);
}