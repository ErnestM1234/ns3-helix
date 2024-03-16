
build_lib_example(
  NAME bittorrent-no-story
  SOURCE_FILES bittorrent-no-story.cc
  LIBRARIES_TO_LINK ${libwifi}
                    ${libbittorrent}
)

build_lib_example(
  NAME bittorrent-1-peer
  SOURCE_FILES bittorrent-1-peer.cc
  LIBRARIES_TO_LINK ${libbittorrent}
)

build_lib_example(
  NAME bittorrent-n-peers
  SOURCE_FILES bittorrent-n-peers.cc
  LIBRARIES_TO_LINK ${libbittorrent}
)

if(${ENABLE_REAL_TIME})
    build_lib_example(
        NAME vodsim
        SOURCE_FILES vodsim.cc
        LIBRARIES_TO_LINK ${libbittorrent}
    )
else()
    build_lib_example(
        NAME vodsim
        SOURCE_FILES vodsim-no-realtime.cc
        LIBRARIES_TO_LINK ${libbittorrent}
    )
endif()



## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def build(bld):
#     obj = bld.create_ns3_program('bittorrent-no-story', ['bittorrent', 'wifi'])
#     obj.source = 'bittorrent-no-story.cc'
    
#     obj = bld.create_ns3_program('vodsim', ['bittorrent'])
    
#     if bld.env['ENABLE_REAL_TIME']:
#         obj.source = 'vodsim.cc'
#     else:
#         obj.source = 'vodsim-no-realtime.cc'
    
