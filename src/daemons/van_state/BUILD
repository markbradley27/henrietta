load("@io_bazel_rules_go//go:def.bzl", "go_binary", "go_library")
load("@rules_pkg//pkg:deb.bzl", "pkg_deb")
load("@rules_pkg//pkg:mappings.bzl", "pkg_attributes", "pkg_filegroup", "pkg_files")
load("@rules_pkg//pkg:tar.bzl", "pkg_tar")

go_library(
    name = "edge_handler",
    srcs = ["edge_handler.go"],
    importpath = "github.com/markbradley27/henrietta/src/daemons/van_state/edge_handler",
    deps = [
        "//src/consts:go_consts",
        "//src/proto:van_state_go_proto",
        "@com_github_eclipse_paho_mqtt_golang//:paho_mqtt_golang",
        "@com_github_warthog618_gpiod//:gpiod",
        "@org_golang_google_protobuf//encoding/prototext",
        "@org_golang_google_protobuf//proto",
    ],
)

go_binary(
    name = "van_state",
    srcs = ["main.go"],
    deps = [
        ":edge_handler",
        "//src/consts:go_consts",
        "//src/daemons:log_util",
        "//src/daemons:mqtt_util",
        "@com_github_warthog618_gpiod//:gpiod",
    ],
)

pkg_files(
    name = "van_state_binary_file",
    srcs = [":van_state"],
    attributes = pkg_attributes(
        mode = "0544",
        user = "pi",
    ),
    prefix = "/usr/local/bin",
)

pkg_files(
    name = "van_state_service_file",
    srcs = ["van_state.service"],
    prefix = "/etc/systemd/system",
)

pkg_filegroup(
    name = "van_state_deb_filegroup",
    srcs = [
        "van_state_binary_file",
        "van_state_service_file",
    ],
)

pkg_tar(
    name = "van_state_deb_tar",
    srcs = [
        ":van_state_deb_filegroup",
    ],
)

pkg_deb(
    name = "van_state_deb",
    out = "van_state.deb",
    architecture = "armhf",
    data = ":van_state_deb_tar",
    description = "Daemon responsible for monitoring van state.",
    maintainer = "Mark Bradley <markbradley27@gmail.com>",
    package = "van_state",
    postinst = "postinst",
    postrm = "postrm",
    prerm = "prerm",
    version = "0.0.1",
)
