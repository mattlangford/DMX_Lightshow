cc_library(
    name="serial_comms",
    srcs=["serial.cc"],
    hdrs=["serial.hh",
          "ftd2xx.h",
          "WinTypes.h"
    ],
    deps=["@ftd2xx_libs//:ftd2xx"]
)

cc_binary(
    name="test_dmx",
    srcs=["test_dmx.cc"],
    deps=[":serial_comms"]
)
