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

cc_library(
    name = "testing",
    srcs = [
        "testing.cpp",
    ],
    hdrs = [
        "testing.hpp",
    ],
    visibility = [
        ":__subpackages__",
        "//python:__subpackages__",
        "//tools/reader:__subpackages__",
    ],
    deps = [
        "//cpp:inspector",
    ],
)

cc_test(
    name = "config_test",
    srcs = [
        "config_test.cpp",
    ],
    deps = [
        "//cpp:inspector",
        "@gtest//:gtest_main",
    ],
)

cc_test(
    name = "logging_test",
    srcs = [
        "logging_test.cpp",
    ],
    deps = [
        "//cpp:inspector",
        "@gtest//:gtest_main",
    ],
)

cc_test(
    name = "system_test",
    srcs = [
        "system_test.cpp",
    ],
    deps = [
        "//cpp:inspector",
        "@gtest//:gtest_main",
    ],
)

cc_test(
    name = "trace_event_test",
    srcs = [
        "trace_event_test.cpp",
    ],
    deps = [
        "//cpp:inspector",
        "@gtest//:gtest_main",
    ],
)

cc_test(
    name = "trace_reader_writer_test",
    srcs = [
        "trace_reader_writer_test.cpp",
    ],
    deps = [
        "//cpp:inspector",
        "//cpp/tests:testing",
        "@gtest//:gtest_main",
    ],
)

cc_test(
    name = "trace_test",
    srcs = [
        "trace_test.cpp",
    ],
    deps = [
        "//cpp:inspector",
        "//cpp/tests:testing",
        "@gtest//:gtest_main",
    ],
)
