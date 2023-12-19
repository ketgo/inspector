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

cc_library(
    name = "priority_queue",
    hdrs = [
        "priority_queue.hpp",
    ],
    deps = [
        "@glog",
    ],
)

cc_test(
    name = "priority_queue_test",
    srcs = [
        "priority_queue_test.cpp",
    ],
    deps = [
        ":priority_queue",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "reader",
    srcs = [
        "reader.cpp",
    ],
    hdrs = [
        "reader.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":priority_queue",
        "//cpp:inspector",
        "@glog",
    ],
)

cc_test(
    name = "reader_test",
    srcs = [
        "reader_test.cpp",
    ],
    deps = [
        ":reader",
        "//cpp/tests:testing",
        "@gtest//:gtest_main",
    ],
)

pybind_library(
    name = "reader_py",
    srcs = [
        "reader_py.cpp",
    ],
    soname = "reader_py",
    visibility = ["//visibility:public"],
    deps = [
        ":reader",
    ],
)

py_test(
    name = "reader_py_test",
    srcs = [
        "reader_py_test.py",
    ],
    deps = [
        ":reader_py",
        "//python:inspector",
    ],
)
