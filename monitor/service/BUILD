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
load("@deps_python//:requirements.bzl", "entry_point", "requirement")

py_library(
    name = "config",
    srcs = [
        "config.py",
    ],
    deps = [],
)

py_library(
    name = "app",
    srcs = glob(["app/**/*.py"]),
    data = ["templates/index.html"],
    imports = ["monitor/service"],
    deps = [
        requirement("flask"),
        requirement("flask-cors"),
        requirement("py-healthcheck"),
        requirement("gevent"),
        requirement("gevent-websocket"),
        requirement("Flask-SocketIO"),
        requirement("Flask-CeleryExt"),
        requirement("python-engineio"),
    ],
)

py_binary(
    name = "server",
    srcs = [
        "server.py",
    ],
    imports = ["monitor/service"],
    main = "server.py",
    deps = [
        ":app",
        ":config",
    ],
)

py_binary(
    name = "worker",
    srcs = [
        "worker.py",
    ],
    imports = ["monitor/service"],
    main = "worker.py",
    deps = [
        ":app",
        ":config",
    ],
)

alias(
    name = "celery",
    actual = entry_point("celery"),
)
