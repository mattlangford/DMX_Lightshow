new_local_repository(
    name = "ftd2xx_libs",
    # pkg-config --variable=libdir x11
    path = "/usr/local/lib",
    build_file_content = """
cc_library(
    name = "ftd2xx",
    srcs = ["libftd2xx.so"],
    visibility = ["//visibility:public"],
)
""",
)
