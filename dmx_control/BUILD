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
    deps=[":serial_comms"]
)

cc_binary(
    name="test_dmx",
    srcs=["test_dmx.cc"],
    deps=[":serial_comms",
          ":dmx_helpers"]
)
