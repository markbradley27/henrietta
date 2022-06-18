load("@platformio_rules//platformio:platformio.bzl", "platformio_library", "platformio_project")

package(default_visibility = ["//visibility:public"])

platformio_library(
    name = "button",
    hdr = "button.h",
)

platformio_library(
    name = "ring_buffer",
    hdr = "ring_buffer.h",
)

platformio_library(
    name = "util",
    hdr = "util.h",
)

platformio_project(
    name = "enviro_micky",
    src = "main.cc",
    board = "nodemcuv2",
    environment_kwargs = {
        "lib_deps": """
            256dpi/MQTT@^2.5.0
            adafruit/Adafruit PM25 AQI Sensor@^1.0.6
            adafruit/Adafruit SSD1306@^2.5.3
            adafruit/Adafruit Unified Sensor@^1.1.5
            adafruit/DHT sensor library@^1.4.3
            arduino-libraries/NTPClient@^3.2.1
            nanopb/Nanopb@^0.4.6
        """,
    },
    framework = "arduino",
    platform = "espressif8266",
    deps = [
        ":button",
        ":ring_buffer",
        ":util",
        "//src/boards/enviro_micky/displayers:ath_big_numbers_displayer",
        "//src/boards/enviro_micky/displayers:ath_raw_displayer",
        "//src/boards/enviro_micky/displayers:graph_displayer",
        "//src/consts:platformio_consts",
        "//src/proto:enviro_platformio_proto",
    ],
)
