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

cc_library(
    name = "event",
    srcs = [
        "event.cpp",
    ],
    hdrs = [
        "event.hpp",
    ],
    deps = [
        "//cpp:inspector",
    ],
)

cc_test(
    name = "event_test",
    srcs = [
        "event_test.cpp",
    ],
    deps = [
        ":event",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "basic_reader",
    srcs = [
        "basic_reader.cpp",
    ],
    hdrs = [
        "basic_reader.hpp",
    ],
    deps = [
        ":event",
    ],
)

cc_test(
    name = "basic_reader_test",
    srcs = [
        "basic_reader_test.cpp",
    ],
    deps = [
        ":basic_reader",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "priority_queue",
    hdrs = [
        "priority_queue.h",
    ],
    deps = [],
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
    srcs = ["reader.cpp"],
    hdrs = [
        "reader.hpp",
    ],
    deps = [
        ":basic_reader",
        ":priority_queue",
    ],
)

cc_test(
    name = "reader_test",
    srcs = [
        "reader_test.cpp",
    ],
    deps = [
        ":reader",
        "@gtest//:gtest_main",
    ],
)

pybind_library(
    name = "reader_py",
    srcs = [
        "reader_py.cpp",
    ],
    soname = "reader_py",
    deps = [
        ":basic_reader",
    ],
)
