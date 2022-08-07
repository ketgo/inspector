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
    name = "reader_test",
    srcs = [
        "reader_test.cpp",
    ],
    deps = [
        "//cpp:inspector",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "trace_event",
    srcs = [
        "trace_event.cpp",
    ],
    hdrs = [
        "trace_event.hpp",
    ],
    visibility = [
        ":__subpackages__",
    ],
    deps = [
        "//utils:strings",
    ],
)

cc_test(
    name = "tracer_test",
    srcs = [
        "tracer_test.cpp",
    ],
    deps = [
        ":trace_event",
        "//cpp:inspector",
        "@gtest//:gtest_main",
    ],
)
