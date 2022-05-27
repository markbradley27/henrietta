load("@io_bazel_rules_go//proto:def.bzl", "go_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")
load(":platformio_proto.bzl", "platformio_proto_library")

package(default_visibility = ["//visibility:public"])

proto_library(
    name = "enviro_proto",
    srcs = ["enviro.proto"],
)

go_proto_library(
    name = "enviro_go_proto",
    importpath = "github.com/markbradley27/henrietta/src/proto/enviro_go_proto",
    proto = ":enviro_proto",
)

platformio_proto_library(
    name = "enviro_platformio_proto",
    proto = "enviro.proto",
)
