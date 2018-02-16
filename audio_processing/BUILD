cc_library(
    name="audio",
    hdrs=["audio.hh",
          "fft_helpers.hh",
          "beat_finders.hh",
    ],
    deps=["@portaudio_libs//:portaudio"]
)

cc_binary(
    name="beats",
    srcs=["beat.cc"],
    deps=[":audio"]
)
