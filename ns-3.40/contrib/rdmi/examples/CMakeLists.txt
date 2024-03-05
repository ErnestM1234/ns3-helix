build_lib_example(
    NAME rdmi-example
    SOURCE_FILES rdmi-example.cc
    LIBRARIES_TO_LINK ${librdmi}
)

build_lib_example(
    NAME rdmi-header-example
    SOURCE_FILES rdmi-header-example.cc
    LIBRARIES_TO_LINK ${librdmi}
)
