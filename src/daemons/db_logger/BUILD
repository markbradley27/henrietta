load("@io_bazel_rules_go//go:def.bzl", "go_binary")

go_binary(
    name = "db_logger",
    srcs = ["main.go"],
    importpath = "github.com/markbradley27/henrietta/src/daemons/db_logger",
    deps = [
        "//src/consts:go_consts",
        "@com_github_eclipse_paho_mqtt_golang//:paho_mqtt_golang"
    ],
)
