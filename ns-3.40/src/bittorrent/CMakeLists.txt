
# # DEPENDENCY_NAME is used as a prefix to variables set by the find_external_library macro
# # HEADER_NAME(S) is(are) the name(s) of the header(s) you want to include
# # LIBRARY_NAME(S) is(are) the name(s) of the library(ies) you want to link
# # SEARCH_PATHS are the custom paths you can give if your library is not on a system path
# find_external_library(DEPENDENCY_NAME helix
#                       HEADER_NAMES model/helix.h helix-socket.h helix-socket-impl.h
#                       LIBRARY_NAME helix-ns3
#                       SEARCH_PATHS /Users/ernestmccarter/Documents/Princeton/School/concentration/senior\ thesis/ns3/workspace/ns-allinone-3.40/ns3-helix)

# # If the header(s) and library(ies) are not found, a message will be printed during the configuration
# # If the header(s) and the library(ies) are found, we can use the information found by the buildsystem
# if(${helix_FOUND}) # Notice that the contents of DEPENDENCY_NAME became a prefix for the _FOUND variable
#     # The compiler will not be able to find the include that is not on
#     # a system include path, unless we explicitly inform it

#     # This is the equivalent of -I/optional/search/path/to/custom/sqlite3/include
#     # and AFFECTS ALL the targets in the CURRENT DIRECTORY and ITS SUBDIRECTORIES
#     include_directories(${helix_INCLUDE_DIRS})

#     # The compiler should be able to locate the headers, but it still needs to be
#     # informed of the libraries that should be linked

#     # This is the equivalent of -l/optional/search/path/to/custom/sqlite3/library/libsqlite3.so
#     # and AFFECTS ALL the targets in the CURRENT DIRECTORY and ITS SUBDIRECTORIES
#     link_libraries(${helix_LIBRARIES})
# endif()



build_lib(
  LIBNAME bittorrent # bittorrent module which can later be linked to examples and modules with ${libbittorrent}
  SOURCE_FILES # equivalent to module.source
	  model/common/3rd-party/sha1.cc
    model/common/BitTorrentUtilities.cc
    model/common/GlobalMetricsGatherer.cc
    model/common/Torrent.cc
    model/common/TorrentFile.cc
    ## Client ##
    model/client/AbstractStrategy.cc
    model/client/BitTorrentClient.cc
    model/client/BitTorrentVideoClient.cc
    model/client/BitTorrentHttpClient.cc
    model/client/BitTorrentPacket.cc
    model/client/BitTorrentPeer.cc
    model/client/BitTorrentVideoMetricsBase.cc
    model/client/ChokeUnChokeStrategyBase.cc
    model/client/PartSelectionStrategyBase.cc
    model/client/PeerConnectorStrategyBase.cc
    model/client/ProtocolFactory.cc
    model/client/RequestSchedulingStrategyBase.cc
    model/client/StorageManager.cc
    model/client/strategies/RarestFirstPartSelectionStrategy.cc
    #model/client/strategies/vod/bitos/BiToS-PartSelectionStrategy.cc
    #model/client/strategies/vod/gtg/GTG-ChokeUnChokeStrategy.cc
    #model/client/strategies/vod/gtg/GTG-PartSelectionStrategy.cc
    #model/client/strategies/vod/rf-vod/RF-VoD-PartSelectionStrategy.cc
    ## Tracker ##
    model/tracker/BitTorrentTracker.cc
    model/tracker/BitTorrentHttpServer.cc
    ## Helpers ##
    helper/VODSimBriteTopologyHelper.cc
    helper/Story.cc
    helper/no-tap/brite-tap-helper.cc # cannot figure out how to enable tap-bridge so doing this option
    # helper/brite-tap-helper.cc
  HEADER_FILES # equivalent to headers.source
    model/common/3rd-party/sha1.h
    model/common/BitTorrentDefines.h
    model/common/BitTorrentUtilities.h
    model/common/GlobalMetricsGatherer.h
    model/common/Torrent.h
    model/common/TorrentFile.h
    ## Client ##
    model/client/AbstractStrategy.h
    model/client/BitTorrentClient.h
    model/client/BitTorrentVideoClient.h
    model/client/BitTorrentHttpClient.h
    model/client/BitTorrentPacket.h
    model/client/BitTorrentPeer.h
    model/client/BitTorrentVideoMetricsBase.h
    model/client/ChokeUnChokeStrategyBase.h
    model/client/PartSelectionStrategyBase.h
    model/client/PeerConnectorStrategyBase.h
    model/client/ProtocolFactory.h
    model/client/RequestSchedulingStrategyBase.h       
    model/client/StorageManager.h
    model/client/strategies/RarestFirstPartSelectionStrategy.h
    #model/client/strategies/vod/bitos/BiToS-PartSelectionStrategy.h
    #model/client/strategies/vod/gtg/GTG-ChokeUnChokeStrategy.h
    #model/client/strategies/vod/gtg/GTG-PartSelectionStrategy.h
    #model/client/strategies/vod/rf-vod/RF-VoD-PartSelectionStrategy.h
    ## Tracker ##
    model/tracker/BitTorrentTracker.h
    model/tracker/BitTorrentHttpServer.h
    ## Helpers ##
    helper/VODSimBriteTopologyHelper.h
    helper/Story.h
    helper/no-tap/brite-tap-helper.h # cannot figure out how to enable tap-bridge so doing this option
    # helper/brite-tap-helper.h
  LIBRARIES_TO_LINK ${libcore}
                    ${libinternet}
                    ${libnetwork}
                    ${libnix-vector-routing}
                    ${libpoint-to-point}
                    ${libtopology-read}
                    ${libcsma}
                    ${libapplications}
                    ${libmpi}
                    # ${libhelix}
                    # /Users/ernestmccarter/Documents/Princeton/School/concentration/senior\ thesis/ns3/workspace/ns-allinone-3.40/helix-ns3/build/libhelix-ns3.dylib
                    # ${tap-bridge}
  TEST_SOURCES
)




# ## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def build(bld):
#     if tap-bridge in bld.env[MODULES_NOT_BUILT]:
#       module = bld.create_ns3_module(bittorrent [core internet network nix-vector-routing point-to-point topology-read csma 
# applications])
#     else:
#       module = bld.create_ns3_module(bittorrent [core internet network nix-vector-routing point-to-point topology-read csma applications tap-bridge])
#       # module = bld.create_ns3_module(bittorrent [core internet network nix-vector-routing point-to-point topology-read csma applications tap-bridge slicetime])
#     module.source = [
#         ## Common ##
# 	model/common/3rd-party/sha1.cc
#         model/common/BitTorrentUtilities.cc
#         model/common/GlobalMetricsGatherer.cc
#         model/common/Torrent.cc
#         model/common/TorrentFile.cc
#         ## Client ##
#     	model/client/AbstractStrategy.cc
#         model/client/BitTorrentClient.cc
#         model/client/BitTorrentVideoClient.cc
#         model/client/BitTorrentHttpClient.cc
#         model/client/BitTorrentPacket.cc
#         model/client/BitTorrentPeer.cc
#         model/client/BitTorrentVideoMetricsBase.cc
#         model/client/ChokeUnChokeStrategyBase.cc
#         model/client/PartSelectionStrategyBase.cc
#         model/client/PeerConnectorStrategyBase.cc
#         model/client/ProtocolFactory.cc
#         model/client/RequestSchedulingStrategyBase.cc
#         model/client/StorageManager.cc
#         model/client/strategies/RarestFirstPartSelectionStrategy.cc
#         #model/client/strategies/vod/bitos/BiToS-PartSelectionStrategy.cc
#         #model/client/strategies/vod/gtg/GTG-ChokeUnChokeStrategy.cc
#         #model/client/strategies/vod/gtg/GTG-PartSelectionStrategy.cc
#         #model/client/strategies/vod/rf-vod/RF-VoD-PartSelectionStrategy.cc
# 	## Tracker ##
# 	model/tracker/BitTorrentTracker.cc
# 	model/tracker/BitTorrentHttpServer.cc
# 	## Helpers ##
# 	helper/VODSimBriteTopologyHelper.cc
#         helper/Story.cc
#         ]

#     headers = bld(features=ns3header)
#     headers.module = bittorrent
#     headers.source = [
#         ## Common ##
# 	model/common/3rd-party/sha1.h
#         model/common/BitTorrentDefines.h
#         model/common/BitTorrentUtilities.h
#         model/common/GlobalMetricsGatherer.h
#         model/common/Torrent.h
#         model/common/TorrentFile.h
#         ## Client ##
#     	model/client/AbstractStrategy.h
#         model/client/BitTorrentClient.h
#         model/client/BitTorrentVideoClient.h
#         model/client/BitTorrentHttpClient.h
#         model/client/BitTorrentPacket.h
#         model/client/BitTorrentPeer.h
#         model/client/BitTorrentVideoMetricsBase.h
#         model/client/ChokeUnChokeStrategyBase.h
#         model/client/PartSelectionStrategyBase.h
#         model/client/PeerConnectorStrategyBase.h
#         model/client/ProtocolFactory.h
#         model/client/RequestSchedulingStrategyBase.h       
#         model/client/StorageManager.h
#         model/client/strategies/RarestFirstPartSelectionStrategy.h
#         #model/client/strategies/vod/bitos/BiToS-PartSelectionStrategy.h
#         #model/client/strategies/vod/gtg/GTG-ChokeUnChokeStrategy.h
#         #model/client/strategies/vod/gtg/GTG-PartSelectionStrategy.h
#         #model/client/strategies/vod/rf-vod/RF-VoD-PartSelectionStrategy.h
#         ## Tracker ##
# 	model/tracker/BitTorrentTracker.h
# 	model/tracker/BitTorrentHttpServer.h
# 	## Helpers ##
#         helper/VODSimBriteTopologyHelper.h
#         helper/Story.h
#         ]
        
#     if tap-bridge in bld.env[MODULES_NOT_BUILT]:
#       module.source.append(helper/no-tap/brite-tap-helper.cc)
#       headers.source.append(helper/no-tap/brite-tap-helper.h)
#     else:
#       module.source.append(helper/brite-tap-helper.cc)
#       headers.source.append(helper/brite-tap-helper.h)
      
#     bld.recurse(examples)
