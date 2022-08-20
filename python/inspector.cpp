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

void BindConfig(py::module& m);
void BindLogging(py::module& m);
void BindReader(py::module& m);
void BindTraceEvent(py::module& m);
void BindTracer(py::module& m);

PYBIND11_MODULE(INSPECTOR_PYTHON_MODULE, m) {
  m.doc() = "Tool set to capture real time application traces for inspection.";

  BindConfig(m);
  BindLogging(m);
  BindReader(m);
  BindTraceEvent(m);
  BindTracer(m);
}