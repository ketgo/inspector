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

#include <inspector/details/config.hpp>
#include <inspector/details/event_queue.hpp>
#include <inspector/details/event_header.hpp>
#include <inspector/details/logging.hpp>
#include <inspector/details/system.hpp>

#include "utils/strings.hpp"

namespace py = pybind11;

/**
 * @brief Method to write a test log.
 *
 */
void WriteTestLog() { LOG_ERROR << "Test Log Message"; }

/**
 * @brief Trace event parser for testing.
 *
 */
struct Event {
  inspector::details::EventHeader header;
  char phase;
  std::string name;
  std::string args;

  static Event Parse(const std::string& data) {
    Event event;

    const auto* header_ =
        reinterpret_cast<const inspector::details::EventHeader*>(data.data());
    event.header.type = header_->type;
    event.header.timestamp = header_->timestamp;
    event.header.pid = header_->pid;
    event.header.tid = header_->tid;

    auto split = utils::SplitOnce(
        data.substr(sizeof(inspector::details::EventHeader)), "|");
    event.phase = split.first[0];
    split = utils::SplitOnce(split.second, "|");
    event.name = split.first;
    event.args = split.second;

    return event;
  }
};

/**
 * @brief Method to consume a test event.
 *
 */
Event ConsumeTestEvent() {
  static auto queue = inspector::details::system::GetSharedObject<
      inspector::details::EventQueue>(
      inspector::details::Config::Get().queue_system_unique_name);
  inspector::details::EventQueue::ReadSpan span;
  queue->Consume(span);
  auto event = std::string(span.Data(), span.Size());
  return Event::Parse(event);
}

/**
 * @brief Binding testing sub-module to the given python module.
 *
 * @param m Reference to the python module.
 */
void BindTesting(py::module& m) {
  auto test_m = m.def_submodule(
      "testing", "Sub-module containing helper methods for testing.");

  test_m.def("write_test_log", &WriteTestLog);

  py::class_<inspector::details::EventHeader>(test_m, "EventHeader")
      .def(py::init<>())
      .def_readonly("type", &inspector::details::EventHeader::type)
      .def_readonly("timestamp", &inspector::details::EventHeader::timestamp)
      .def_readonly("pid", &inspector::details::EventHeader::pid)
      .def_readonly("tid", &inspector::details::EventHeader::tid);

  py::class_<Event>(test_m, "Event")
      .def_static("load", &Event::Parse)
      .def(py::init<>())
      .def_readonly("header", &Event::header)
      .def_readonly("phase", &Event::phase)
      .def_readonly("name", &Event::name)
      .def_readonly("args", &Event::args);

  test_m.def("consume_test_event", &ConsumeTestEvent);
}