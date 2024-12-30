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
    srcs = [
        "common.cpp",
    ],
    hdrs = [
        "common.hpp",
    ],
    deps = [
        "@perfetto//:trace_cc_proto",
    ],
)


cc_library(
    name = "generator",
    srcs = [
        "generator.cpp",
    ],
    hdrs = [
        "generator.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":common",
        "//cpp:inspector",
        "//tools/common/storage",
        "@glog",
    ],
)

pybind_library(
    name = "generator_py",
    srcs = [
        "generator_py.cpp",
    ],
    imports = ["."],
    visibility = ["//visibility:public"],
    soname = "generator_py",
    deps = [
        ":generator",
        "@glog",
    ],
)


cc_binary(
    name = "run",
    srcs = [
        "run.cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "generator",
        "@glog",
    ],
)