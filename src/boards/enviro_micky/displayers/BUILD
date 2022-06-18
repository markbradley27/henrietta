load("@platformio_rules//platformio:platformio.bzl", "platformio_library")

package(default_visibility = ["//visibility:public"])

platformio_library(
    name = "displayer",
    hdr = "displayer.h",
)

platformio_library(
    name = "aqi_displayer",
    hdr = "aqi_displayer.h",
    deps = [
        ":displayer",
        "//src/boards/enviro_micky:ring_buffer",
        "//src/boards/enviro_micky:util",
    ],
)

platformio_library(
    name = "ath_big_numbers_displayer",
    hdr = "ath_big_numbers_displayer.h",
    deps = [
        ":displayer",
        "//src/boards/enviro_micky:ring_buffer",
        "//src/boards/enviro_micky:util",
    ],
)

platformio_library(
    name = "ath_raw_displayer",
    hdr = "ath_raw_displayer.h",
    deps = [
        ":displayer",
        "//src/boards/enviro_micky:ring_buffer",
        "//src/boards/enviro_micky:util",
    ],
)

platformio_library(
    name = "graph_displayer",
    hdr = "graph_displayer.h",
    deps = [
        ":displayer",
        "//src/boards/enviro_micky:ring_buffer",
    ],
)

