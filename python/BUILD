# Copyright 2022 Ketan Goyal
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load("//third_party/pybind11:defs.bzl", "pybind_library")

package(default_visibility = ["//visibility:public"])

pybind_library(
    name = "inspector",
    srcs = [
        "src/config.cpp",
        "src/inspector.cpp",
        "src/logging.cpp",
        "src/testing.cpp",
        "src/trace.cpp",
        "src/trace_event.cpp",
    ],
    imports = ["."],
    soname = "inspector",
    deps = [
        "//cpp:inspector",
        "//cpp/tests:testing",
    ],
)
