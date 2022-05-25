load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

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
    name = "com_github_go_yaml_yaml",
    importpath = "github.com/go-yaml/yaml",
    tag = "v3.0.0",
)

# Required for paho.mqtt.golang.
go_repository(
		name = "com_github_gorilla_websocket",
		importpath = "github.com/gorilla/websocket",
		tag = "v1.5.0",
)

# Required for paho.mqtt.golang.
#go_repository(
#		name = "org_golang_x_net_proxy",
#		importpath = "golang.org/x/net/proxy",
#		tag = "v0.0.0-20220520000938-2e3eb7b945c2",
#)

go_repository(
		name = "com_github_eclipse_paho_mqtt_golang",
		importpath = "github.com/eclipse/paho.mqtt.golang",
		tag = "v1.3.5",
)

go_rules_dependencies()

go_register_toolchains(version = "1.18.2")

gazelle_dependencies()
