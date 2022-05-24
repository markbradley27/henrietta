load("@io_bazel_rules_go//go:def.bzl", "go_binary", "go_library")

package(default_visibility = ["//visibility:public"])

go_binary(
    name = "gen_consts",
    srcs = ["gen_consts.go"],
)

genrule(
    name = "consts",
    srcs = ["consts.json"],
    outs = [
        "consts.h",
        "consts.go",
    ],
    cmd = "$(location :gen_consts) --json_path $< --c_out $(RULEDIR)/consts.h --go_out $(RULEDIR)/consts.go",
    tools = [":gen_consts"],
)

go_library(
    name = "go_consts",
    srcs = ["consts.go"],
    embedsrcs = [":consts"],
    importpath = "github.com/henrietta/src/consts/go_consts",
)

go_library(
    name = "consts_lib",
    srcs = [
        "consts.go",
        "consts.h",
        "gen_consts.go",
    ],
    importpath = "github.com/markbradley27/henrietta/src/consts",
)
