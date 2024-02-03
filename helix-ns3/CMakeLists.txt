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
pkg_check_modules(helix-ns3-bridge REQUIRED IMPORTED_TARGET helix-ns3-bridge)

target_link_libraries(helix-ns3
    PkgConfig::ns3-core
    PkgConfig::ns3-point-to-point
    PkgConfig::ns3-internet
    PkgConfig::ns3-network
    PkgConfig::ns3-bridge
    PkgConfig::ns3-traffic-control
    PkgConfig::ns3-applications
    PkgConfig::helix-ns3-bridge)

include(GNUInstallDirs)

add_executable(helix-example examples/helix-example.cc)

target_include_directories(helix-example PRIVATE .)

target_link_libraries(helix-example helix-ns3)

# Install both the helix-ns3 library, as well as all binaries
install(TARGETS helix-ns3 helix-example
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

