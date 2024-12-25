load("@rules_python//python:defs.bzl", "py_binary")

cc_binary(
    name = "recorder",
    srcs = [
        "recorder.cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cpp:inspector",
        "//tools/common/storage",
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