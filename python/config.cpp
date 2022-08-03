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

#include <inspector/config.hpp>

namespace py = pybind11;

/**
 * @brief Binding the C++ config module to the given python module.
 *
 * @param m Reference to the python module.
 */
void BindConfig(py::module& m) {
  py::class_<inspector::Config>(m, "Config")
      .def(py::init())
      .def_readwrite("EVENT_QUEUE_SYSTEM_UNIQUE_NAME",
                     &inspector::Config::queue_system_unique_name)
      .def_readwrite("MAX_ATTEMPT", &inspector::Config::max_attempt)
      .def_readwrite("REMOVE", &inspector::Config::remove);
}