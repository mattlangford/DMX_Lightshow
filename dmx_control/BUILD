cc_library(
    name="serial_comms",
    srcs=["serial.cc"],
    hdrs=["serial.hh"],
    deps=["@ftd2xx_libs//:ftd2xx"]
)

cc_library(
    name="dmx_helpers",
    srcs=["dmx.cc"],
    hdrs=["dmx.hh"],
    visibility = ["//visibility:public"],
    deps=[":serial_comms"]
)

cc_library(
    name="light_universe_controller",
    srcs=["light_universe_controller.cc"],
    hdrs=["light_universe_controller.hh"],
    visibility = ["//visibility:public"],
    deps=["//dmx_control/lights:light_base"]
)

cc_library(
    name="light_effects",
    srcs=[],
    hdrs=["light_effects.hh"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name="test_dmx",
    srcs=["test_dmx.cc"],
    deps=[":serial_comms",
          ":dmx_helpers",
          "//dmx_control/lights:litake_basic_light",
          ":light_universe_controller"]
)
cc_binary(
    name="test_serial",
    srcs=["test_serial.cc"],
    deps=[":serial_comms"]
)
