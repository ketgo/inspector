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
    name = "common",
    hdrs = [
        "common.hpp",
    ],
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
        ":file_io"
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
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "writer",
    srcs = [
        "writer.cpp",
    ],
    hdrs = [
        "writer.hpp",
    ],
    deps = [
        ":block",
        ":common",
        ":file_io"
    ],
)