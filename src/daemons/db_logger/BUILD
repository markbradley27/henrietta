load("@io_bazel_rules_go//go:def.bzl", "go_binary", "go_library")
load("@rules_pkg//pkg:deb.bzl", "pkg_deb")
load("@rules_pkg//pkg:mappings.bzl", "pkg_attributes", "pkg_filegroup", "pkg_files")
load("@rules_pkg//pkg:tar.bzl", "pkg_tar")

go_library(
    name = "mqtt_handler",
    srcs = ["mqtt_handler.go"],
    importpath = "github.com/markbradley27/henrietta/src/daemons/db_logger/mqtt_handler",
    deps = [
        "//src/proto:enviro_go_proto",
        "//src/proto:van_state_go_proto",
        "@com_github_eclipse_paho_mqtt_golang//:paho_mqtt_golang",
        "@org_golang_google_protobuf//encoding/prototext",
        "@org_golang_google_protobuf//proto",
    ],
)

go_binary(
    name = "db_logger",
    srcs = ["main.go"],
    deps = [
        ":mqtt_handler",
        "//src/consts:go_consts",
        "//src/daemons:log_util",
        "//src/daemons:mqtt_util",
        "@com_github_eclipse_paho_mqtt_golang//:paho_mqtt_golang",
        "@com_github_lib_pq//:pq",
    ],
)

pkg_files(
    name = "db_logger_binary_file",
    srcs = [":db_logger"],
    attributes = pkg_attributes(
        mode = "0544",
        user = "pi",
    ),
    prefix = "/usr/local/bin",
)

pkg_files(
    name = "db_logger_service_file",
    srcs = ["db_logger.service"],
    prefix = "/etc/systemd/system",
)

pkg_filegroup(
    name = "db_logger_deb_filegroup",
    srcs = [
        "db_logger_binary_file",
        "db_logger_service_file",
    ],
)

pkg_tar(
    name = "db_logger_deb_tar",
    srcs = [
        ":db_logger_deb_filegroup",
    ],
)

pkg_deb(
    name = "db_logger_deb",
    out = "db_logger.deb",
    architecture = "armhf",
    data = ":db_logger_deb_tar",
    description = "Daemon responsible for logging henrietta data to postgres.",
    maintainer = "Mark Bradley <markbradley27@gmail.com>",
    package = "db_logger",
    postinst = "postinst",
    postrm = "postrm",
    prerm = "prerm",
    version = "0.0.1",
)
