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

#include <inspector/writer.hpp>

namespace py = pybind11;

/**
 * @brief Binding event writer to the given python module.
 *
 * @param m Reference to the python module.
 */
void BindWriter(py::module& m) {
  py::class_<inspector::Writer>(m, "Writer")
      .def_static("set_config", &inspector::Writer::SetConfig)
      .def(py::init())
      .def("write", &inspector::Writer::Write,
           "Write the given event to the shared event queue.");
}