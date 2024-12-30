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
    name = "common",
    hdrs = [
        "common.hpp",
    ],
)

cc_library(
    name = "checksum", 
    srcs = [
        "checksum.cpp",
    ],
    hdrs = [
        "checksum.hpp",
    ],
    deps= []
)

cc_test(
    name = "checksum_test",
    srcs = [
        "checksum_test.cpp",
    ],
    deps = [
        ":checksum",
        "@gtest//:gtest_main",
    ]
)

cc_library(
    name = "testing",
    srcs = [
        "testing.cpp",
    ],
    hdrs = [
        "testing.hpp",
    ],
    deps = [
        "//utils:tempdir",
        "@boost//:filesystem",
    ],
)


cc_library(
    name = "file_io",
    srcs = [
        "file_io.cpp",
    ],
    hdrs = [
        "file_io.hpp",
    ],
    deps = [
        ":common",
    ],
)

cc_test(
    name = "file_io_test",
    srcs = [
        "file_io_test.cpp",
    ],
    data = [
        "file_io_test.data",
    ],
    deps = [
        ":file_io",
        ":testing",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "block",
    srcs = [
        "block.cpp",
    ],
    hdrs = [
        "block.hpp",
    ],
    deps = [
        ":common",
        ":file_io",
        ":checksum",
    ],
)

cc_test(
    name = "block_test",
    srcs = [
        "block_test.cpp",
    ],
    deps = [
        ":block",
        ":testing",
        "//utils:random",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "storage",
    srcs = [
        "storage.cpp",
    ],
    visibility = ["//visibility:public"],
    hdrs = [
        "storage.hpp",
    ],
    deps = [
        ":block",
        ":common",
        ":file_io"
    ],
)

cc_test(
    name = "storage_test",
    srcs = [
        "storage_test.cpp",
    ],
    deps = [
        ":storage",
        ":testing",
        "//utils:random",
        "@gtest//:gtest_main",
    ],
)


pybind_library(
    name = "storage_py",
    srcs = [
        "storage_py.cpp",
    ],
    soname = "storage_py",
    visibility = ["//visibility:public"],
    deps = [
        ":storage",
    ],
)

py_test(
    name = "storage_py_test",
    srcs = [
        "storage_py_test.py",
    ],
    deps = [
        ":storage_py",
    ],
)
