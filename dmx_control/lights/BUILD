cc_library(
    name="light_base",
    hdrs=["light_base.hh"],
    deps=["//dmx_control:dmx_helpers"]
)

cc_library(
    name="litake_basic_light",
    srcs=["litake_basic_light.cc"],
    hdrs=["litake_basic_light.hh"],
    visibility = ["//visibility:public"],
    deps=[":light_base"]
)
