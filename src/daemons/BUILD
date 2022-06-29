load("@io_bazel_rules_go//go:def.bzl", "go_library")

package(default_visibility = [":__subpackages__"])

go_library(
    name = "log_util",
    srcs = ["log_util.go"],
    importpath = "github.com/markbradley27/henrietta/src/daemons/log_util",
)

go_library(
    name = "mqtt_util",
    srcs = ["mqtt_util.go"],
    importpath = "github.com/markbradley27/henrietta/src/daemons/mqtt_util",
    deps = [
        "@com_github_eclipse_paho_mqtt_golang//:paho_mqtt_golang",
    ],
)
