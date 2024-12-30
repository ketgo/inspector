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

workspace(name = "inspector")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# --------------------------------------
# Python Rules
# --------------------------------------

http_archive(
    name = "rules_python",
    sha256 = "4f7e2aa1eb9aa722d96498f5ef514f426c1f55161c3c9ae628c857a7128ceb07",
    strip_prefix = "rules_python-1.0.0",
    url = "https://github.com/bazelbuild/rules_python/releases/download/1.0.0/rules_python-1.0.0.tar.gz",
)

load("@rules_python//python:repositories.bzl", "py_repositories")

py_repositories()

load("@rules_python//python:repositories.bzl", "python_register_toolchains")

python_register_toolchains(
    name = "python_3_8",
    python_version = "3.8.10",
)

load("@rules_python//python:pip.bzl", "pip_parse")

pip_parse(
    name = "deps_python",
    python_interpreter_target = "@python_3_8_host//:python",
    requirements_lock = "//third_party/python:requirements_lock.txt",
)

load("@deps_python//:requirements.bzl", "install_deps")

install_deps()

# --------------------------------------
# PyBind11
# --------------------------------------

http_archive(
    name = "pybind11_bazel",
    sha256 = "fec6281e4109115c5157ca720b8fe20c8f655f773172290b03f57353c11869c2",
    strip_prefix = "pybind11_bazel-72cbbf1fbc830e487e3012862b7b720001b70672",
    urls = ["https://github.com/pybind/pybind11_bazel/archive/72cbbf1fbc830e487e3012862b7b720001b70672.zip"],
)

http_archive(
    name = "pybind11",
    build_file = "@pybind11_bazel//:pybind11.BUILD",
    sha256 = "eacf582fa8f696227988d08cfc46121770823839fe9e301a20fbce67e7cd70ec",
    strip_prefix = "pybind11-2.10.0",
    urls = ["https://github.com/pybind/pybind11/archive/v2.10.0.tar.gz"],
)

load("@pybind11_bazel//:python_configure.bzl", "python_configure")

python_configure(
    name = "local_config_python",
    python_interpreter_target = "@python_3_8_host//:python",
)

# ----------------------------------------
# Protobuf
# ----------------------------------------

_PROTO_VERSION="29.1"

http_archive(
    name = "com_google_protobuf",
    strip_prefix = "protobuf-{}".format(_PROTO_VERSION),
    integrity = "sha256-PTKUDpdcStm4umlkDnj1UnB1uuM8ookCdb8muFPAliw=",
    urls = ["https://github.com/protocolbuffers/protobuf/releases/download/v{}/protobuf-{}.tar.gz".format(_PROTO_VERSION, _PROTO_VERSION)],
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

# --------------------------------------
# Gtest
# --------------------------------------

http_archive(
    name = "gtest",
    sha256 = "ab78fa3f912d44d38b785ec011a25f26512aaedc5291f51f3807c592b506d33a",
    strip_prefix = "googletest-58d77fa8070e8cec2dc1ed015d66b454c8d78850",
    urls = ["https://github.com/google/googletest/archive/58d77fa8070e8cec2dc1ed015d66b454c8d78850.zip"],
)

# ---------------------------------------
# Glog
# ---------------------------------------

http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-2.2.2",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
)

http_archive(
    name = "glog",
    sha256 = "122fb6b712808ef43fbf80f75c52a21c9760683dae470154f02bddfc61135022",
    strip_prefix = "glog-0.6.0",
    urls = ["https://github.com/google/glog/archive/v0.6.0.zip"],
)

# ----------------------------------------
# Bigcat IPC
# ----------------------------------------

git_repository(
    name = "bigcat_ipc",
    remote = "git@github.com:ketgo/bigcat_ipc.git",
    commit = "93cec3522cd1c1c4dfb03cfdf5e3873aeaad5f3e"
)

# ----------------------------------------
# Boost
# ----------------------------------------

_RULES_BOOST_COMMIT = "ce2b65fd6d1494aadb2d8c99ce26aa222ab72486"

http_archive(
    name = "com_github_nelhage_rules_boost",
    sha256 = "f3038ed0e19f68920396fbe1a824cde8ab321e131c9fe59826f8ee510b958569",
    strip_prefix = "rules_boost-%s" % _RULES_BOOST_COMMIT,
    urls = [
        "https://github.com/nelhage/rules_boost/archive/%s.tar.gz" % _RULES_BOOST_COMMIT,
    ],
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()

# ----------------------------------------
# Perfetto
# ----------------------------------------

_PERFETTO_VERSION="48.1"

http_archive(
    name = "perfetto",
    strip_prefix = "perfetto-%s" % _PERFETTO_VERSION,
    urls = [
        "https://github.com/google/perfetto/archive/refs/tags/v%s.tar.gz" % _PERFETTO_VERSION,
    ],
    integrity = "sha256-jRxr9E8b2wmKtwzWDaPOa25zHk6yHdUrJSfL3PhdmE0="
)

local_repository(
    name = "perfetto_cfg",
    path = "third_party/perfetto/overrides",
)