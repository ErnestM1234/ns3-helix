build_lib_example(
    NAME helix-example
    SOURCE_FILES helix-example.cc
    LIBRARIES_TO_LINK ${libhelix}
)

build_lib_example(
    NAME helix-large-transfer
    SOURCE_FILES helix-large-transfer.cc
    LIBRARIES_TO_LINK ${libhelix}
                      ${libpoint-to-point}
                      ${libapplications}
                      ${libinternet}
)

