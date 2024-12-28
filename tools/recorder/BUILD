load("@rules_python//python:defs.bzl", "py_binary")

cc_library(
    name = "base",
    srcs = [
        "base.cpp",
    ],
    hdrs = [
        "base.hpp",
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
        ":base",
         "//cpp:inspector",
         "//tools/common/storage",
    ],
)

cc_binary(
    name = "run",
    srcs = [
        "run.cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        ":trace_recorder",
        "@glog",
    ],
)

py_binary(
    name = "wrapper",
    srcs = [
        "wrapper.py",
    ],
    data = [
        "@inspector//tools/recorder:recorder",
    ],
    visibility = ["//visibility:public"],
    deps = [],
)

py_binary(
    name = "experimental",
    srcs = [
        "experimental.py",
    ],
    deps = [],
)