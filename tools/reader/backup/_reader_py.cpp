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

#include <inspector/reader.hpp>

namespace py = pybind11;

/**
 * @brief Binding event reader to the given python module.
 *
 * @param m Reference to the python module.
 */
void BindReader(py::module& m) {
  py::class_<inspector::Reader>(m, "Reader")
      .def(py::init())
      .def(
          "__iter__",
          [](const inspector::Reader& reader) {
            return py::make_iterator(reader.begin(), reader.end());
          },
          py::keep_alive<0, 1>());
}