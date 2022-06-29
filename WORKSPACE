load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# PlatformIO Rules
git_repository(
    name = "platformio_rules",
    remote = "http://github.com/mum4k/platformio_rules.git",
    tag = "v0.0.10",
)

# Package Rules (deb)
http_archive(
    name = "rules_pkg",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_pkg/releases/download/0.7.0/rules_pkg-0.7.0.tar.gz",
        "https://github.com/bazelbuild/rules_pkg/releases/download/0.7.0/rules_pkg-0.7.0.tar.gz",
    ],
    sha256 = "8a298e832762eda1830597d64fe7db58178aa84cd5926d76d5b744d6558941c2",
)
load("@rules_pkg//:deps.bzl", "rules_pkg_dependencies")
rules_pkg_dependencies()

# Protobuf
http_archive(
    name = "com_google_protobuf",
    sha256 = "d0f5f605d0d656007ce6c8b5a82df3037e1d8fe8b121ed42e536f569dec16113",
    strip_prefix = "protobuf-3.14.0",
    urls = [
        "https://mirror.bazel.build/github.com/protocolbuffers/protobuf/archive/v3.14.0.tar.gz",
        "https://github.com/protocolbuffers/protobuf/archive/v3.14.0.tar.gz",
    ],
)
load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")
protobuf_deps()

# Go (and go libraries)
http_archive(
    name = "io_bazel_rules_go",
    sha256 = "ab21448cef298740765f33a7f5acee0607203e4ea321219f2a4c85a6e0fb0a27",
    url = "https://github.com/bazelbuild/rules_go/releases/download/v0.32.0/rules_go-v0.32.0.zip",
)

http_archive(
    name = "bazel_gazelle",
    sha256 = "62ca106be173579c0a167deb23358fdfe71ffa1e4cfdddf5582af26520f1c66f",
    url = "https://github.com/bazelbuild/bazel-gazelle/releases/download/v0.23.0/bazel-gazelle-v0.23.0.tar.gz",
)

load("@io_bazel_rules_go//go:deps.bzl", "go_register_toolchains", "go_rules_dependencies")
load("@bazel_gazelle//:deps.bzl", "gazelle_dependencies", "go_repository")

go_repository(
    name = "com_github_eclipse_paho_mqtt_golang",
    importpath = "github.com/eclipse/paho.mqtt.golang",
    tag = "v1.3.5",
)

go_repository(
    name = "com_github_go_yaml_yaml",
    importpath = "github.com/go-yaml/yaml",
    tag = "v3.0.0",
)

go_repository(
    name = "org_golang_google_protobuf",
    importpath = "google.golang.org/protobuf",
    tag = "v1.28.0",
)

# Required by paho.mqtt.golang.
go_repository(
    name = "com_github_gorilla_websocket",
    importpath = "github.com/gorilla/websocket",
    tag = "v1.5.0",
)

go_repository(
    name = "com_github_lib_pq",
    importpath = "github.com/lib/pq",
    tag = "v1.10.4",
)

go_repository(
    name = "com_github_warthog618_gpiod",
    importpath = "github.com/warthog618/gpiod",
    tag = "v0.8.0",
)

go_rules_dependencies()

go_register_toolchains(version = "1.18.2")

gazelle_dependencies()
