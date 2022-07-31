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

#include <inspector/logging.hpp>

namespace py = pybind11;

/**
 * @brief Adaptor for python logging module.
 *
 */
class PythonLogger : public inspector::Logger {
 public:
  PythonLogger(py::object& log);
  void operator<<(const std::string& message) override;

 private:
  py::object log_;
};

// ---------------------------------
// PythonLogger Implementation
// ---------------------------------

PythonLogger::PythonLogger(py::object& log) : log_(log) {}

void PythonLogger::operator<<(const std::string& message) { log_(message); }

// ---------------------------------

/**
 * @brief Binding the C++ logging module to the given python module.
 *
 * @param m Reference to the python module.
 */
void BindLogging(py::module& m) {
  auto logging = py::module::import("logging");
  auto py_logger = logging.attr("getLogger")(m.attr("__name__"));
  static PythonLogger info_logger(py_logger.attr("info")),
      warn_logger(py_logger.attr("warn")),
      error_logger(py_logger.attr("error"));
  inspector::RegisterLogger(inspector::LogLevel::INFO, info_logger);
  inspector::RegisterLogger(inspector::LogLevel::WARN, warn_logger);
  inspector::RegisterLogger(inspector::LogLevel::ERROR, error_logger);
}
