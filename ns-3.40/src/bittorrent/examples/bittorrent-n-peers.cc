/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010-2012 ComSys, RWTH Aachen University
 * Derived from the sixth tutorial setup.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Rene Glebke
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
// #include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/trace-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"


#include "ns3/BitTorrentTracker.h"
#include "ns3/BitTorrentVideoClient.h"
#include "ns3/GlobalMetricsGatherer.h"


using namespace ns3;
using namespace bittorrent;


NS_LOG_COMPONENT_DEFINE("BittorrentOnePeer");


/**
 * Callback function for when a unicast packet is received by tracker node
 * Not sure if this is the cleanest way of doing things, but I saw
 * this method suggested in a form post by the one guy who answers every
 * form post question, so this should be okay for this experiement.
 * Justification: https://groups.google.com/g/ns-3-users/c/8pn2eBxbM3w
 * 
 * I would much rather prefer that forwarding is just disabled overall
 * for that node, but I am not sure how this is done without causing
 * PopulateGlobalRoutingTables() to segfault.
*/

void
TrackerForwardingCallback(const Ipv4Header& ipv4Header, Ptr<const ns3::Packet> p, uint32_t interface_id)
{
    Ipv4Address src = ipv4Header.GetSource();
    Ipv4Address dest = ipv4Header.GetDestination();
    NS_LOG_DEBUG(Simulator::Now().GetSeconds() << " Tracker: " << src << " > " << dest << " if: " << interface_id);
}

void
ChokerForwardingCallback(const Ipv4Header& ipv4Header, Ptr<const ns3::Packet> p, uint32_t interface_id)
{
    Ipv4Address src = ipv4Header.GetSource();
    Ipv4Address dest = ipv4Header.GetDestination();
    NS_LOG_DEBUG(Simulator::Now().GetSeconds()  << " Choker: " << src << " > " << dest << " if: " << interface_id);
}


void
SeedSendCallback(const Ipv4Header& ipv4Header, Ptr<const ns3::Packet> p, uint32_t interface_id)
{
    if (interface_id == 1) return;
    Ipv4Address src = ipv4Header.GetSource();
    Ipv4Address dest = ipv4Header.GetDestination();
    NS_LOG_DEBUG(Simulator::Now().GetSeconds()  << " Seed Send: " << src << " > " << dest << " if: " << interface_id);
}

void
PeerSendCallback(const Ipv4Header& ipv4Header, Ptr<const ns3::Packet> p, uint32_t interface_id)
{
    if (interface_id == 1) return;
    Ipv4Address src = ipv4Header.GetSource();
    Ipv4Address dest = ipv4Header.GetDestination();
    NS_LOG_DEBUG(Simulator::Now().GetSeconds()  << " Peer Send: " << src << " > " << dest << " if: " << interface_id);
}

void
PeerRecvCallback(const Ipv4Header& ipv4Header, Ptr<const ns3::Packet> p, uint32_t interface_id)
{
    Ipv4Address src = ipv4Header.GetSource();
    Ipv4Address dest = ipv4Header.GetDestination();
    NS_LOG_DEBUG(Simulator::Now().GetSeconds()  << " Peer Recv: " << src << " > " << dest << " if: " << interface_id);
}

void
TrackerSendCallback(const Ipv4Header& ipv4Header, Ptr<const ns3::Packet> p, uint32_t interface_id)
{
    Ipv4Address src = ipv4Header.GetSource();
    Ipv4Address dest = ipv4Header.GetDestination();
    NS_LOG_DEBUG(Simulator::Now().GetSeconds() << " Tracker Send: " << src << " > " << dest << " if: " << interface_id);
}

void
TrackerRecvCallback(const Ipv4Header& ipv4Header, Ptr<const ns3::Packet> p, uint32_t interface_id)
{
    Ipv4Address src = ipv4Header.GetSource();
    Ipv4Address dest = ipv4Header.GetDestination();
    NS_LOG_DEBUG(Simulator::Now().GetSeconds() << " Tracker Recv: " << src << " > " << dest << " if: " << interface_id);
}

// get corresponding choke indecies (a is closer peer, b is further peer)
uint32_t
GetChIndex(uint32_t a, uint32_t b, uint32_t n)
{
    return (n-1)*b + (a>b) + a;
}

int main (int argc, char *argv[])
{
    /*
    * This example contains one peer and a connection with a chokepoint between the
    * seed and the peer.
    */

    // LogComponentEnable("BittorrentOnePeer", LOG_LEVEL_DEBUG);
    // LogComponentEnable("BittorrentPeer", LOG_LEVEL_INFO);
    // export NS_LOG="BittorrentPeer=level_info|node|time"
    // LogComponentEnable("BittorrentBitTorrentClient", LOG_LEVEL_DEBUG);
    // LogComponentEnable("BittorrentPeerConnectorStrategyBase", LOG_LEVEL_INFO);
    // export NS_LOG="BittorrentPeerConnectorStrategyBase=level_debug|node|time"
    // LogComponentEnable("BittorrentPartSelectionStrategyBase", LOG_DEBUG);
    // LogComponentEnable("BittorrentChokeUnChokeStrategyBase", LOG_LEVEL_INFO);
    // export NS_LOG="BittorrentChokeUnChokeStrategyBase=level_info|node|time"
    // LogComponentEnable("BitTorrentTracker", LOG_LEVEL_INFO);
    // export NS_LOG="BitTorrentTracker=level_info|node|time"
    // LogComponentEnable("BitTorrentHttpServer", LOG_LEVEL_INFO);
    // export NS_LOG="BitTorrentHttpServer=level_info|node|time"
    // BitTorrentHttpClient
    // export NS_LOG="BitTorrentTracker=level_debug|node|time:BittorrentPeerConnectorStrategyBase=level_debug|node|time"
    // export NS_LOG="BitTorrentTracker=level_warn|node|time:BittorrentPeerConnectorStrategyBase=level_debug|node|time"
    // export NS_LOG="BitTorrentHttpServer=level_debug|node|time:BittorrentPeerConnectorStrategyBase=level_debug|node|time"
    // export NS_LOG="BitTorrentHttpServer=level_debug|node|time:BittorrentPeerConnectorStrategyBase=level_debug|node|time:BitTorrentHttpClient=level_debug|node|time"
    // export NS_LOG=""

    uint32_t peer_count = 5; // number of peers >=2 (1 seeder, 1 peer)
    uint32_t seeder_count = 1; // peer_count = seeder_count + leecher_count
    uint32_t endHostCount = peer_count+1;


    NS_LOG_DEBUG("Creating Nodes");
    // Create Nodes
    NodeContainer peers;    // tracker is at index 0, it's technically not a peer
    peers.Create (endHostCount);
    NodeContainer chokers;
    chokers.Create (endHostCount);

    // Install Internet Stack
    InternetStackHelper internet;
    internet.Install(peers);
    internet.Install(chokers);


    NS_LOG_DEBUG("Creating Point to Point Connections");
    // add connections from peers to larger network
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.255");
    for (uint32_t i = 0; i < endHostCount; i++)
    {
        // create connection between peer/tracker and choker
        NodeContainer peerChokerContainer = NodeContainer(peers.Get(i), chokers.Get(i));
        PointToPointHelper p2pPeerChoker;
        p2pPeerChoker.SetDeviceAttribute("DataRate", StringValue("1Gbps")); // High bandwidth
        p2pPeerChoker.SetChannelAttribute("Delay", StringValue("0ms"));
        NetDeviceContainer peerChokerDeviceContainer = p2pPeerChoker.Install(peers.Get(i), chokers.Get(i));

        // add addressing
        Ipv4InterfaceContainer peerChokerIfContainer;
        peerChokerIfContainer = ipv4.Assign(peerChokerDeviceContainer);
    }

    // create n2n fully connected network (must be a diff loop to write all peer choker connections first)
    for (uint32_t i = 0; i < endHostCount; i++)
    {
        for (uint32_t j = i+1; j < endHostCount; j++)
        {
            // create connection between peer and choker
            NodeContainer peerChokerContainer;
            peerChokerContainer = NodeContainer(chokers.Get(i), chokers.Get(j));
            PointToPointHelper p2pChokerChoker;
            if (i == 0)  // tracker (high bandwidth)
            {
                p2pChokerChoker.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
                p2pChokerChoker.SetChannelAttribute("Delay", StringValue("0ms"));
            }
            else // choke point (variable bandwidth)
            {
                p2pChokerChoker.SetDeviceAttribute("DataRate", StringValue("800kbps"));
                p2pChokerChoker.SetChannelAttribute("Delay", StringValue("0ms"));
            }
            NetDeviceContainer chokerChokerDeviceContainer;
            chokerChokerDeviceContainer = p2pChokerChoker.Install(peerChokerContainer);

            // add addressing
            Ipv4InterfaceContainer chokerChokerIfContainer;
            chokerChokerIfContainer = ipv4.Assign(chokerChokerDeviceContainer);
        }
    }

    // populate routing tables (needs to be separate cuz until now didnt know all IPs)
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    for (uint32_t i = 0; i < endHostCount; i++)
    {
        // get peer i and choker i's routing table
        Ptr<Ipv4StaticRouting> peerRoutingTable = ipv4RoutingHelper.GetStaticRouting(peers.Get(i)->GetObject<Ipv4>());
        Ptr<Ipv4StaticRouting> chokerRoutingTable = ipv4RoutingHelper.GetStaticRouting(chokers.Get(i)->GetObject<Ipv4>());

        for (uint32_t j = 0; j < endHostCount; j++)
        {
            if (i == j) continue;

            // get peer j's ipv4 address
            Ptr<Ipv4> peerJIpv4 = peers.Get(j)->GetObject<Ipv4>();
            Ipv4Address peerJIpv4Address = peerJIpv4->GetAddress(1,0).GetAddress(); // i think this is how bittorent chooses its ip
            // get choker j's routing table
            Ptr<Ipv4StaticRouting> chokerJRoutingTable = ipv4RoutingHelper.GetStaticRouting(chokers.Get(j)->GetObject<Ipv4>());

            // get (choker i to j)'s netdevice corresponding interface index
            // ASSUMPTION: netdevices are indexed in the order they're added
            uint32_t cicjIF = j - (i<j) + 2; // choker i -> choker j

            // add routing path from i to j
            peerRoutingTable->AddHostRouteTo(peerJIpv4Address, 1); // peers only have 1 interface // TODO: instead, use a mask to point to other nodes?
            chokerRoutingTable->AddHostRouteTo(peerJIpv4Address, cicjIF);
            chokerJRoutingTable->AddHostRouteTo(peerJIpv4Address, 1); // ASSUMPTION: interface to a peer will always be 1 (0 is loop back)
        }
    }

    // // Print routing tables
    // Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>(&std::cout);
    // Ipv4RoutingHelper::PrintRoutingTableAllAt(Seconds(1), routingStream);

    // set up tracing system
    // peers.Get(0)->GetObject<Ipv4L3Protocol>()->TraceConnectWithoutContext("LocalDeliver", MakeCallback(&TrackerRecvCallback));
    // peers.Get(0)->GetObject<Ipv4L3Protocol>()->TraceConnectWithoutContext("UnicastForward", MakeCallback(&TrackerForwardingCallback));
    // peers.Get(0)->GetObject<Ipv4L3Protocol>()->TraceConnectWithoutContext("SendOutgoing", MakeCallback(&TrackerSendCallback));
    // peers.Get(1)->GetObject<Ipv4L3Protocol>()->TraceConnectWithoutContext("LocalDeliver", MakeCallback(&PeerRecvCallback));
    // peers.Get(1)->GetObject<Ipv4L3Protocol>()->TraceConnectWithoutContext("SendOutgoing", MakeCallback(&PeerSendCallback));
    
    /*
    * Setting up a BitTorrent simulation is done as follows:
    *
    * 1) Install and configure BitTorrentTracker application on one of the nodes.
    * 2) Load a torrent file via the tracker application.
    * 3) Install BitTorrentClient applications on the desired number of nodes.
    * 4) Set up the BitTorrent metrics gatherer for output handling.
    * 5) Start the simulation.
    */

    NS_LOG_DEBUG("Installing BitTorrent...");

    // 1) Install a BitTorrentTracker application (with default values) on first peer
    NS_LOG_DEBUG("(1) Add Tracker");
    Ptr<BitTorrentTracker> bitTorrentTracker = Create<BitTorrentTracker> ();
    peers.Get (0)->AddApplication (bitTorrentTracker);


    // 2) Load a torrent file via the tracker application
    NS_LOG_DEBUG("(2) Load torrent file");
    Ptr<Torrent> sharedTorrent = bitTorrentTracker->AddTorrent ("input/bittorrent/torrent-data", "input/bittorrent/torrent-data/10MB-full.dat.torrent");

    // 3) Install BitTorrentClient applications on the desired number of nodes
    NS_LOG_DEBUG("(3) Install BitTorrent Client");
    ApplicationContainer bitTorrentClients;
    ApplicationContainer bitTorrentSeeders;
    ApplicationContainer bitTorrentLeechers;
    for (unsigned int i = 1; i < peers.GetN (); i++)
    {
        // Add the client application to the node
        Ptr<BitTorrentClient> client = Create<BitTorrentClient> ();
        client->SetTorrent (sharedTorrent);
        peers.Get (i)->AddApplication (client);
        bitTorrentClients.Add (client);
        if (i <= seeder_count)  // seeder
        {
            // Make this node a seeder (they must also have bittorrent client installed)
            DynamicCast<BitTorrentClient> (peers.Get(1)->GetApplication (0))->SetInitialBitfield ("full");
            // Add to list of seeders
            bitTorrentSeeders.Add(client);
        }
        else    // leecher
        {
            // Add to list of leechers
            bitTorrentLeechers.Add(client);
        }
    }

    // 4) Set up the BitTorrent metrics gatherer for output handling (here, we just log to the screen)
    NS_LOG_DEBUG("(4) Set up metrics");
    GlobalMetricsGatherer* gatherer = GlobalMetricsGatherer::GetInstance ();
    gatherer->SetFileNamePrefix ("This will be ignored while logging to the screen", false);
    gatherer->RegisterWithApplications (bitTorrentClients);
    gatherer->SetStopFraction (1.0, 1.0); // Stops the simulation when all nodes have finished downloading

    // 5) Start the simulation
    // 5a) Make all non-seeding peers delayed in starting slightly, this prevents a 60 second delay
    // because seeder has to send a start and completed announcement which only some of the
    // other peers will see
    bitTorrentTracker->SetStartTime(MilliSeconds(0));
    bitTorrentSeeders.Start(MilliSeconds(0));
    bitTorrentLeechers.Start(MilliSeconds(1));

    // 5b) Run the simulation
    NS_LOG_DEBUG("Starting Simulation...");
    Simulator::Run ();
    Simulator::Destroy ();
    
    return 0;
}
