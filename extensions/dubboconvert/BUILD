licenses(["notice"])  # Apache 2

load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_package",
)

envoy_package()

envoy_cc_library(
    name = "dubbo_test_lib",
    srcs = [
        "config.cc",
        "plugin.cc",
    ],
    hdrs = [
        "plugin.h",
    ],
    repository = "@envoy",
    visibility = ["//visibility:public"],
    deps = [
        "//extensions/common:context",
        "//extensions/common:json_util",
        "//extensions/common:proto_util",
        "@envoy//source/common/common:base64_lib",
        "@envoy//source/extensions/common/wasm/ext:declare_property_cc_proto",
        "@proxy_wasm_cpp_host//:null_lib",
    ],
)

exports_files(["base64.h"])
