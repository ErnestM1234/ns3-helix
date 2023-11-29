CMAKE_MINIMUM_REQUIRED(VERSION 3.25)

project(helix-ns3
    VERSION 0.0.1
    DESCRIPTION "ns-3 module providing a HELIX protocol implementation")

add_library(helix-ns3 SHARED
    model/helix-l4-protocol.cc
    model/helix-rs-interface.cc
    model/helix-socket-factory-impl.cc
    model/helix-socket-factory.cc
    model/helix-socket-impl.cc
    model/helix-socket.cc
    model/helix.cc
    helper/helix-helper.cc)

set_target_properties(helix-ns3 PROPERTIES VERSION ${PROJECT_VERSION})

set(HELIX_NS3_PUBLIC_HEADERS
    "model/helix-l4-protocol.h"
    "model/helix-rs-interface.h"
    "model/helix-socket-factory-impl.h"
    "model/helix-socket-factory.h"
    "model/helix-socket-impl.h"
    "model/helix-socket.h"
    "model/helix.h"
    "helper/helix-helper.h")

# TODO: separate out public from private headers?
set_target_properties(helix-ns3 PROPERTIES PUBLIC_HEADER
    "${HELIX_NS3_PUBLIC_HEADERS}")

target_include_directories(helix-ns3 PRIVATE .)

find_package(PkgConfig)

pkg_check_modules(ns3-core REQUIRED IMPORTED_TARGET ns3-core)
pkg_check_modules(ns3-point-to-point REQUIRED IMPORTED_TARGET ns3-point-to-point)
pkg_check_modules(ns3-internet REQUIRED IMPORTED_TARGET ns3-internet)
pkg_check_modules(ns3-network REQUIRED IMPORTED_TARGET ns3-network)
pkg_check_modules(ns3-bridge REQUIRED IMPORTED_TARGET ns3-bridge)
pkg_check_modules(ns3-traffic-control REQUIRED IMPORTED_TARGET ns3-traffic-control)
pkg_check_modules(ns3-applications REQUIRED IMPORTED_TARGET ns3-applications)

target_link_libraries(helix-ns3
    PkgConfig::ns3-core
    PkgConfig::ns3-point-to-point
    PkgConfig::ns3-internet
    PkgConfig::ns3-network
    PkgConfig::ns3-bridge
    PkgConfig::ns3-traffic-control
    PkgConfig::ns3-applications
    /nix/store/z6fp1lx063ww3dc4gckg83cyyrbjbp1v-helix-rs-0.0.1/lib/libhelix_rs.dylib)

include(GNUInstallDirs)

install(TARGETS helix-ns3
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# option(NS3_COLORED_OUTPUT "Colorize CMake messages" OFF)
# set(PROJECT_SOURCE_DIR "$ENV{NS3_SOURCE_DIR}")
# # Make the Cmake modules to build custom ns-3 modules available:
# list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/build-support")
# include(macros-and-definitions)

# include(CheckIncludeFileCXX)

# CHECK_INCLUDE_FILE_CXX(stdint.h HAVE_STDINT_H)
# if(HAVE_STDINT_H)
#     add_definitions(-DHAVE_STDINT_H)
# endif()

# set(examples_as_tests_sources)
# if(${ENABLE_EXAMPLES})
#     set(examples_as_tests_sources
#         #test/helix-examples-test-suite.cc
#         )
# endif()

# build_lib(
#     LIBNAME helix
#     SOURCE_FILES model/helix-l4-protocol.cc
#                  model/helix-rs-interface.cc
#                  model/helix-socket-factory-impl.cc
#                  model/helix-socket-factory.cc
#                  model/helix-socket-impl.cc
#                  model/helix-socket.cc
#                  model/helix.cc
#                  helper/helix-helper.cc
#     HEADER_FILES model/helix-l4-protocol.h
#                  model/helix-rs-interface.h
#                  model/helix-socket-factory-impl.h
#                  model/helix-socket-factory.h
#                  model/helix-socket-impl.h
#                  model/helix-socket.h
#                  model/helix.h
#                  helper/helix-helper.h
#     LIBRARIES_TO_LINK
#         ${libcore}
#         ${libpoint-to-point}
#         ${libinternet}
#         ${libnetwork}
#         ${libbridge}
#         ${libtraffic-control}
#         ${libapplications}
#         # /Users/ernestmccarter/Documents/Princeton/School/concentration/senior\ thesis/ns3/workspace/ns-allinone-3.40/helix-rs/target/debug/libhelix_rs.dylib
#     TEST_SOURCES test/helix-test-suite.cc
#                  ${examples_as_tests_sources}
# )

