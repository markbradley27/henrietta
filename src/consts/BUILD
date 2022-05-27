load("@io_bazel_rules_go//go:def.bzl", "go_binary", "go_library")
load(":platformio_consts.bzl", "platformio_consts")

package(default_visibility = ["//visibility:public"])

genrule(
    name = "gen_go_consts",
    srcs = ["consts.yaml"],
    outs = [
        "consts.go",
    ],
    cmd = "python3 $(location :gen_consts.py) --yaml_path $< --go_out $@",
    tools = ["gen_consts.py"],
)

go_library(
    name = "go_consts",
    srcs = ["consts.go"],
    embedsrcs = [":gen_go_consts"],
    importpath = "github.com/markbradley27/henrietta/src/consts/go_consts",
)

platformio_consts(
    name = "platformio_consts",
)
