/**
 * Copyright 2023 Ketan Goyal
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

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "tools/recorder/recorder.hpp"

namespace py = pybind11;

PYBIND11_MODULE(INSPECTOR_PYTHON_MODULE, m) {
  FLAGS_logtostderr = 0;
  google::InitGoogleLogging("recorder_py");

  m.doc() = "Recording tool to capture real time application traces.";

  m.def("start_recorder", &inspector::tools::startRecorder, "Start recorder.",
        py::arg("out"), py::arg("block") = false);
  m.def("stop_recorder", &inspector::tools::stopRecorder, "Stop recorder.",
        py::arg("block") = false);
}