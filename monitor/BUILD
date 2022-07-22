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

load("@rules_python//python:defs.bzl", "py_library")
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

cc_library(
    name = "consumer_cc",
    srcs = [
        "consumer.cpp",
    ],
    hdrs = [
        "consumer.hpp",
    ],
    deps = [
        "//cpp:inspector",
    ],
)

cc_test(
    name = "consumer_test",
    srcs = [
        "consumer_test.cpp",
    ],
    deps = [
        ":consumer_cc",
        "@gtest//:gtest_main",
    ],
)

pybind_extension(
    name = "consumer",
    srcs = [
        "consumer_py.cpp",
    ],
    deps = [
        ":consumer_cc",
    ],
)

py_library(
    name = "consumer_py",
    data = [":consumer.so"],
)

py_binary(
    name = "run",
    srcs = [
        "main.py",
    ],
    main = "main.py",
    deps = [
        ":consumer_py",
    ],
)
