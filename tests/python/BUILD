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

load("//third_party/python:defs.bzl", "pytest_test")

pytest_test(
    name = "writer_reader_test",
    srcs = [
        "writer_reader_test.py",
    ],
    deps = [
        "//python:inspector",
    ],
)

pytest_test(
    name = "logging_test",
    srcs = [
        "logging_test.py",
    ],
    deps = [
        "//python:inspector",
    ],
)

pytest_test(
    name = "tracer_test",
    srcs = [
        "tracer_test.py",
    ],
    deps = [
        "//python:inspector",
    ],
)
