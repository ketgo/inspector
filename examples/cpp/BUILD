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
    name = "init",
    srcs = [
        "init.cpp",
    ],
    hdrs = [
        "init.hpp",
    ],
    deps = [
        "//cpp:inspector",
        "@glog",
    ],
)

cc_library(
    name = "periodic_task",
    srcs = [
        "periodic_task.cpp",
    ],
    hdrs = [
        "periodic_task.hpp",
    ],
    deps = [
        "//cpp:inspector",
        "@boost//:asio",
        "@glog",
    ],
)

cc_library(
    name = "number_generator",
    srcs = [
        "number_generator.cpp",
    ],
    hdrs = [
        "number_generator.hpp",
    ],
    deps = [
        "//cpp:inspector",
        "@glog",
    ],
)

cc_binary(
    name = "run",
    srcs = [
        "main.cpp",
    ],
    deps = [
        ":init",
        ":number_generator",
        ":periodic_task",
    ],
)
