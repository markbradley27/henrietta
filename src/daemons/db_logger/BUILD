load("@io_bazel_rules_go//go:def.bzl", "go_binary")
load("@rules_pkg//pkg:deb.bzl", "pkg_deb")
load("@rules_pkg//pkg:mappings.bzl", "pkg_attributes", "pkg_filegroup", "pkg_files")
load("@rules_pkg//pkg:tar.bzl", "pkg_tar")

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

pkg_files(
    name = "db_logger_binary_file",
    srcs = [":db_logger"],
    prefix = "/usr/local/bin",
    attributes = pkg_attributes(
        user = "pi",
        mode = "0544",
    ),
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
    architecture = "armhf",
    data = ":db_logger_deb_tar",
    description = "Daemon responsible for logging henrietta data to postgres.",
    maintainer = "Mark Bradley <markbradley27@gmail.com>",
    out = "db_logger.deb",
    package = "db_logger",
    postinst = "postinst",
    postrm = "postrm",
    prerm = "prerm",
    version = "0.0.1",
)
