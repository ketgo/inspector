load("@com_google_protobuf//bazel:proto_library.bzl", "proto_library")

proto_library(
    name = "import_test_proto",
    srcs = ["import_test.proto"],
    deps = ["@com_google_protobuf//:timestamp_proto"],
)

cc_proto_library(
    name = "import_test_cc_proto",
    deps = [":import_test_proto"],
)

cc_test(
    name="import_test",
    srcs = [
        "import_test.cpp",
    ],
    deps = [
        ":import_test_cc_proto",
    ],
)