check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/rdmi-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME rdmi
    SOURCE_FILES model/rdmi-buffer.cc
                 model/rdmi-header.cc
                 model/rdmi-node.cc
                 model/rdmi-peer.cc
                 model/rdmi-socket.cc
                 model/rdmi.cc
                 helper/rdmi-helper.cc
    HEADER_FILES model/rdmi-buffer.h
                 model/rdmi-header.h
                 model/rdmi-node.h
                 model/rdmi-peer.h
                 model/rdmi-socket.h
                 model/rdmi.h
                 helper/rdmi-helper.h
    LIBRARIES_TO_LINK
        ${libcore}
        ${libinternet}
        ${libnetwork}
        ${libapplications}
    TEST_SOURCES test/rdmi-test-suite.cc
                 ${examples_as_tests_sources}
)

