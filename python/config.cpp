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

namespace py = pybind11;

/**
 * @brief Binding the C++ config module to the given python module.
 *
 * @param m Reference to the python module.
 */
void BindConfig(py::module& m) {
  using ConfigPtr = std::unique_ptr<inspector::details::Config, py::nodelete>;
  py::class_<inspector::details::Config, ConfigPtr>(m, "Config")
      .def(py::init(
          []() { return ConfigPtr(&inspector::details::Config::Get()); }))
      .def_readwrite("EVENT_QUEUE_NAME",
                     &inspector::details::Config::queue_system_unique_name)
      .def_readwrite("REMOVE_EVENT_QUEUE_ON_EXIT",
                     &inspector::details::Config::queue_remove_on_exit)
      .def_readwrite("WRITE_MAX_ATTEMPT",
                     &inspector::details::Config::write_max_attempt)
      .def_readwrite("READ_MAX_ATTEMPT",
                     &inspector::details::Config::read_max_attempt);
}