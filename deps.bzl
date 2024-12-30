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

"""
Load dependencies needed for importing inspector.
"""

load("@bazel_tools//tools/build_defs/repo:local.bzl", "local_repository")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def inspector_deps():
    """
    Load dependencies needed by inspector.
    """

    if not native.existing_rule("com_google_protobuf"):
        _PROTO_VERSION="29.1"

        http_archive(
            name = "com_google_protobuf",
            strip_prefix = "protobuf-{}".format(_PROTO_VERSION),
            integrity = "sha256-PTKUDpdcStm4umlkDnj1UnB1uuM8ookCdb8muFPAliw=",
            urls = ["https://github.com/protocolbuffers/protobuf/releases/download/v{}/protobuf-{}.tar.gz".format(_PROTO_VERSION, _PROTO_VERSION)],
        )

    if not native.existing_rule("com_github_gflags_gflags"):
        http_archive(
            name = "com_github_gflags_gflags",
            sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
            strip_prefix = "gflags-2.2.2",
            urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
        )

    if not native.existing_rule("glog"):
        http_archive(
            name = "glog",
            sha256 = "122fb6b712808ef43fbf80f75c52a21c9760683dae470154f02bddfc61135022",
            strip_prefix = "glog-0.6.0",
            urls = ["https://github.com/google/glog/archive/v0.6.0.zip"],
        )

    if not native.existing_rule("bigcat_ipc"):
        git_repository(
            name = "bigcat_ipc",
            remote = "git@github.com:ketgo/bigcat_ipc.git",
            commit = "93cec3522cd1c1c4dfb03cfdf5e3873aeaad5f3e"
        )

    if not native.existing_rule("com_github_nelhage_rules_boost"):
        _RULES_BOOST_COMMIT = "ce2b65fd6d1494aadb2d8c99ce26aa222ab72486"

        http_archive(
            name = "com_github_nelhage_rules_boost",
            sha256 = "f3038ed0e19f68920396fbe1a824cde8ab321e131c9fe59826f8ee510b958569",
            strip_prefix = "rules_boost-%s" % _RULES_BOOST_COMMIT,
            urls = [
                "https://github.com/nelhage/rules_boost/archive/%s.tar.gz" % _RULES_BOOST_COMMIT,
            ],
        )

    if not native.existing_rule("perfetto"):
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