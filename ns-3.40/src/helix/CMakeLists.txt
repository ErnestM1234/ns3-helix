check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/helix-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME helix
    SOURCE_FILES model/helix-l4-protocol.cc
                 model/helix-rs-interface.cc
                 model/helix-socket-factory-impl.cc
                 model/helix-socket-factory.cc
                 model/helix-socket-impl.cc
                 model/helix-socket.cc
                 model/helix.cc
                 helper/helix-helper.cc
    HEADER_FILES model/helix-l4-protocol.h
                 model/helix-rs-interface.h
                 model/helix-socket-factory-impl.h
                 model/helix-socket-factory.h
                 model/helix-socket-impl.h
                 model/helix-socket.h
                 model/helix.h
                 helper/helix-helper.h
    LIBRARIES_TO_LINK
        ${libcore}
        ${libpoint-to-point}
        ${libinternet}
        ${libnetwork}
        ${libbridge}
        ${libtraffic-control}
        ${libapplications}
        # /Users/ernestmccarter/Documents/Princeton/School/concentration/senior\ thesis/ns3/workspace/ns-allinone-3.40/helix-rs/target/debug/libhelix_rs.dylib
        /Users/ernestmccarter/Documents/Princeton/School/concentration/senior\ thesis/ns3/workspace/ns-allinone-3.40/helix-rs/result/lib/libhelix_rs.dylib
    TEST_SOURCES test/helix-test-suite.cc
                 ${examples_as_tests_sources}
)

