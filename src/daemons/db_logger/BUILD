load("@io_bazel_rules_go//go:def.bzl", "go_binary")

go_binary(
    name = "db_logger",
    srcs = ["main.go"],
    deps = [
        "//src/consts:go_consts",
        "//src/proto:enviro_go_proto",
        "@com_github_eclipse_paho_mqtt_golang//:paho_mqtt_golang",
        "@org_golang_google_protobuf//encoding/prototext",
        "@org_golang_google_protobuf//proto",
        "@org_golang_google_protobuf//types/known/timestamppb",
    ],
)
