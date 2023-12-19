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

load("@deps_python//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_test")

def pytest_test(
        name,
        srcs,
        deps = [],
        args = [],
        **kwargs):
    py_test(
        name = name,
        srcs = [
            "//third_party/python:pytest_wrapper.py",
        ] + srcs,
        python_version = "PY3",
        srcs_version = "PY3",
        deps = [
            requirement("pytest"),
        ] + deps,
        args = [
            "--capture=no",
        ] + args + ["$(location :%s)" % x for x in srcs],
        main = "//third_party/python:pytest_wrapper.py",
        **kwargs
    )
