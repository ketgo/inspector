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
#include <pybind11/chrono.h>

#include <inspector/logging.hpp>

#include "tools/reader/event.hpp"
#include "tools/reader/basic_reader.hpp"
#include "tools/reader/reader.hpp"

namespace py = pybind11;

// TODO: Need an approach to send singnal to reader. If the reader is waiting
// then the python script freezes since the threads dont know that a SIGTERM or
// SIGINT has been recieved.

namespace {

/**
 * @brief Adaptor for python logging module.
 *
 */
class PythonLogger : public inspector::Logger {
 public:
  PythonLogger(const std::string& level, py::object& logger);
  void operator<<(const std::string& message) override;

 private:
  const std::string level_;
  py::object logger_;
};

// ---------------------------------
// PythonLogger Implementation
// ---------------------------------

PythonLogger::PythonLogger(const std::string& level, py::object& logger)
    : level_(level), logger_(logger) {}

void PythonLogger::operator<<(const std::string& message) {
  logger_.attr(level_.c_str())(message);
}

// ---------------------------------

}  // namespace

PYBIND11_MODULE(INSPECTOR_PYTHON_MODULE, m) {
  m.doc() =
      "Reader module to load trace events published by the inspector lib.";

  // ------------------------------------------------------
  // Binding Logging
  // ------------------------------------------------------

  auto logging = py::module::import("logging");
  auto py_logger = logging.attr("getLogger")(m.attr("__name__"));

  static PythonLogger info_logger("info", py_logger);
  inspector::RegisterLogger(inspector::LogLevel::INFO, info_logger);

  static PythonLogger warn_logger("warn", py_logger);
  inspector::RegisterLogger(inspector::LogLevel::WARN, warn_logger);

  static PythonLogger error_logger("error", py_logger);
  inspector::RegisterLogger(inspector::LogLevel::ERROR, error_logger);

  // Unregister python logger before interpreter exits. This is required since
  // the C++ static objects have lifetime greater than that of the interpreter
  // and any logging done in these objects will cause segfault once the
  // interpreter terminates.
  auto atexit = py::module_::import("atexit");
  atexit.attr("register")(py::cpp_function(&inspector::UnregsiterAllLoggers));

  // ------------------------------------------------------

  // ------------------------------------------------------
  // Binding Event
  // ------------------------------------------------------

  py::class_<inspector::Event>(m, "Event")
      .def(py::init())
      .def_property_readonly("type", &inspector::Event::Type)
      .def_property_readonly("timestamp", &inspector::Event::Timestamp)
      .def_property_readonly("pid", &inspector::Event::Pid)
      .def_property_readonly("tid", &inspector::Event::Tid)
      .def_property_readonly("payload", &inspector::Event::Payload)
      .def("__str__", [](const inspector::Event& self) {
        std::stringstream stream;
        stream << R"({"type":)" << (int)self.Type() << R"(,"timestamp":)"
               << self.Timestamp() << R"(,"pid":)" << self.Pid() << R"(,"tid":)"
               << self.Tid() << R"(,"payload":")" << self.Payload() << R"("})";
        return stream.str();
      });

  // ------------------------------------------------------

  // ------------------------------------------------------
  // Binding Basic Reader
  // ------------------------------------------------------

  py::class_<inspector::BasicReader>(m, "BasicReader")
      .def(py::init())
      .def(py::init<const std::string&, const std::size_t>(),
           py::arg("queue_name") =
               inspector::details::Config::Get().queue_system_unique_name,
           py::arg("max_read_attempt") =
               inspector::details::Config::Get().max_read_attempt)
      .def(
          "__iter__",
          [](const inspector::BasicReader& reader) {
            return py::make_iterator(reader.begin(), reader.end());
          },
          py::keep_alive<0, 1>());

  // ------------------------------------------------------

  // ------------------------------------------------------
  // Binding Reader
  // ------------------------------------------------------

  py::class_<inspector::Reader>(m, "Reader")
      .def(py::init())
      .def(py::init<const std::string&, const std::size_t,
                    const std::chrono::microseconds&, const std::size_t,
                    const int64_t>(),
           py::arg("queue_name") =
               inspector::details::Config::Get().queue_system_unique_name,
           py::arg("max_read_attempt") =
               inspector::details::Config::Get().max_read_attempt,
           py::arg("polling_interval") =
               inspector::Reader::DefaultPollingInterval(),
           py::arg("worker_count") = inspector::Reader::DefaultWorkerCount(),
           py::arg("buffer_window_size") =
               inspector::Reader::DefaultBufferWindowSize())
      .def(py::init<const std::chrono::microseconds&, const std::string&,
                    const std::size_t, const std::chrono::microseconds&,
                    const std::size_t, const int64_t>(),
           py::arg("timeout"),
           py::arg("queue_name") =
               inspector::details::Config::Get().queue_system_unique_name,
           py::arg("max_read_attempt") =
               inspector::details::Config::Get().max_read_attempt,
           py::arg("polling_interval") =
               inspector::Reader::DefaultPollingInterval(),
           py::arg("worker_count") = inspector::Reader::DefaultWorkerCount(),
           py::arg("buffer_window_size") =
               inspector::Reader::DefaultBufferWindowSize())
      .def(
          "__iter__",
          [](const inspector::Reader& reader) {
            return py::make_iterator(reader.begin(), reader.end());
          },
          py::keep_alive<0, 1>());

  // ------------------------------------------------------
}