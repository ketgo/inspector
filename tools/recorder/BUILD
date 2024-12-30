load("@rules_python//python:defs.bzl", "py_binary")
load("//third_party/pybind11:defs.bzl", "pybind_library")


cc_library(
    name = "collector_base",
    srcs = [
        "collector_base.cpp",
    ],
    hdrs = [
        "collector_base.hpp",
    ],
    deps = [
        "//cpp:inspector",
    ],
)

cc_library(
    name = "storage_collector",
    srcs = [
        "storage_collector.cpp",
    ],
    hdrs = [
        "storage_collector.hpp",
    ],
    deps = [
        "collector_base",
        "//tools/common/storage",
    ],
)


cc_library(
    name = "recorder_base",
    srcs = [
        "recorder_base.cpp",
    ],
    hdrs = [
        "recorder_base.hpp",
    ],
    deps = [
         "@glog",
    ],
)

cc_library(
    name = "trace_recorder",
    srcs = [
        "trace_recorder.cpp",
    ],
    hdrs = [
        "trace_recorder.hpp",
    ],
    deps = [
        ":recorder_base",
        ":collector_base",
        "//cpp:inspector",
    ],
)

cc_library(
    name = "recorder",
    srcs = [
        "recorder.cpp",
    ],
    hdrs = [
        "recorder.hpp",
    ],
    deps = [
        ":trace_recorder",
        ":storage_collector",
        "@glog",
    ],
)

pybind_library(
    name = "recorder_py",
    srcs = [
        "recorder_py.cpp",
    ],
    imports = ["."],
    soname = "recorder_py",
    deps = [
        ":recorder",
        "@glog",
    ],
)


cc_binary(
    name = "run",
    srcs = [
        "run.cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":recorder",
        "@glog",
    ],
)

py_binary(
    name = "wrapper",
    srcs = [
        "wrapper.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":recorder_py",
        "//tools/viewers/perfetto:generator_py",
    ],
)
