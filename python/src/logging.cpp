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
  PythonLogger(const std::string &level, py::object &logger);
  void operator<<(const std::string &message) override;

 private:
  const std::string level_;
  py::object logger_;
};

// ---------------------------------
// PythonLogger Implementation
// ---------------------------------

PythonLogger::PythonLogger(const std::string &level, py::object &logger)
    : level_(level), logger_(logger) {}

void PythonLogger::operator<<(const std::string &message) {
  logger_.attr(level_.c_str())(message);
}

// ---------------------------------

/**
 * @brief Binding the C++ logging module to the given python module.
 *
 * @param m Reference to the python module.
 */
void bindLogging(py::module &m) {
  auto logging = py::module::import("logging");
  auto py_logger = logging.attr("getLogger")(m.attr("__name__"));

  inspector::registerLogger(inspector::LogLevel::INFO,
                            std::make_shared<PythonLogger>("info", py_logger));
  inspector::registerLogger(inspector::LogLevel::WARN,
                            std::make_shared<PythonLogger>("warn", py_logger));
  inspector::registerLogger(inspector::LogLevel::ERROR,
                            std::make_shared<PythonLogger>("error", py_logger));

  // Unregister python logger before interpreter exits. This is required since
  // the C++ static objects have lifetime greater than that of the interpreter
  // and any logging done in these objects will cause segfault once the
  // interpreter terminates.
  auto atexit = py::module_::import("atexit");
  atexit.attr("register")(py::cpp_function(&inspector::unregisterAllLoggers));
}
