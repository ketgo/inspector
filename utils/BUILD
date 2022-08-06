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

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "random",
    hdrs = [
        "random.hpp",
    ],
    deps = [],
)

cc_library(
    name = "threads",
    srcs = [
        "threads.cpp",
    ],
    hdrs = [
        "threads.hpp",
    ],
    deps = [],
)

cc_library(
    name = "chrono",
    hdrs = [
        "chrono.hpp",
    ],
    deps = [
        ":random",
    ],
)

cc_library(
    name = "dprint",
    srcs = [
        "dprint.cpp",
    ],
    hdrs = [
        "dprint.hpp",
    ],
    deps = [],
)

cc_library(
    name = "strings",
    srcs = [
        "strings.cpp",
    ],
    hdrs = [
        "strings.hpp",
    ],
    deps = [],
)
