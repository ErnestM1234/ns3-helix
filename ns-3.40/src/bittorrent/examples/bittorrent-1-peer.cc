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


int main (int argc, char *argv[])
{
    /*
    * This example contains one peer and a connection with a chokepoint between the
    * seed and the peer.
    */

    LogComponentEnable("BittorrentOnePeer", LOG_LEVEL_DEBUG);
    // LogComponentEnable("BittorrentPeer", LOG_LEVEL_INFO);
    // export NS_LOG="BittorrentPeer=level_info|node|time"
    // LogComponentEnable("BittorrentBitTorrentClient", LOG_LEVEL_INFO);
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


    NS_LOG_DEBUG("Creating Nodes");
    NodeContainer seeds;
    seeds.Create (1);
    NodeContainer peers; // this actually refers to leechers, not peers
    peers.Create (1);
    NodeContainer chokers;
    chokers.Create (2); // 0 is seed choker and 1 is peer choker (not to be confused with BitTorrent chocking)
    NodeContainer trackers;
    trackers.Create (1);


    // Data Transfer Point to Point
    NS_LOG_DEBUG("Creating Point to Point Connections");
    PointToPointHelper seedToChoker;
    seedToChoker.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    seedToChoker.SetChannelAttribute("Delay", StringValue("0ms")); // TODO: adjust value

    PointToPointHelper peerToChoker;
    peerToChoker.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    peerToChoker.SetChannelAttribute("Delay", StringValue("0ms"));

    PointToPointHelper chokerToChoker;
    chokerToChoker.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    chokerToChoker.SetChannelAttribute("Delay", StringValue("0ms"));

    // Metadata Transfer Point to Point (ie, tracker related)
    PointToPointHelper seedToTracker;
    seedToTracker.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    seedToTracker.SetChannelAttribute("Delay", StringValue("0ms"));

    PointToPointHelper peerToTracker;
    peerToTracker.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    peerToTracker.SetChannelAttribute("Delay", StringValue("0ms"));

    // Install net devices on each node
    NetDeviceContainer seedToChokerDevices; 
    seedToChokerDevices = seedToChoker.Install(seeds.Get(0), chokers.Get(0));
    NetDeviceContainer peerToChokerDevices;
    peerToChokerDevices = peerToChoker.Install(peers.Get(0), chokers.Get(1));
    NetDeviceContainer chokerToChokerDevices;
    chokerToChokerDevices = chokerToChoker.Install(chokers);
    NetDeviceContainer seedToTrackerDevices;
    seedToTrackerDevices = seedToTracker.Install(seeds.Get(0), trackers.Get(0));
    NetDeviceContainer peerToTrackerDevices;
    peerToTrackerDevices = peerToTracker.Install(peers.Get(0), trackers.Get(0));

    
    // Install internet stack
    NS_LOG_DEBUG("Installing Internet Stack");
    InternetStackHelper stack;
    stack.Install(seeds);
    stack.Install(peers);
    stack.Install(chokers);
    stack.Install(trackers);
    // disable forwarding on tracker, so packets must use chokepoint (using callbacks to filter)
    NS_LOG_DEBUG("Add tracking for choker and tracker");
    // trackers.Get(0)->GetObject<Ipv4L3Protocol>()->TraceConnectWithoutContext("LocalDeliver", MakeCallback(&TrackerRecvCallback));
    // trackers.Get(0)->GetObject<Ipv4L3Protocol>()->TraceConnectWithoutContext("UnicastForward", MakeCallback(&TrackerForwardingCallback));
    // trackers.Get(0)->GetObject<Ipv4L3Protocol>()->TraceConnectWithoutContext("SendOutgoing", MakeCallback(&TrackerSendCallback));
    // chokers.Get(0)->GetObject<Ipv4L3Protocol>()->TraceConnectWithoutContext("UnicastForward", MakeCallback(&ChokerForwardingCallback));
    // seeds.Get(0)->GetObject<Ipv4L3Protocol>()->TraceConnectWithoutContext("SendOutgoing", MakeCallback(&SeedSendCallback));
    // peers.Get(0)->GetObject<Ipv4L3Protocol>()->TraceConnectWithoutContext("LocalDeliver", MakeCallback(&PeerRecvCallback));
    // peers.Get(0)->GetObject<Ipv4L3Protocol>()->TraceConnectWithoutContext("SendOutgoing", MakeCallback(&PeerSendCallback));


    // Set up addressing
    NS_LOG_DEBUG("Installing Addresses");
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer seedToChokerInterfaces;
    seedToChokerInterfaces = address.Assign(seedToChokerDevices);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer peerToChokerInterfaces;
    peerToChokerInterfaces = address.Assign(peerToChokerDevices);

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer chokerToChokerInterfaces;
    chokerToChokerInterfaces = address.Assign(chokerToChokerDevices);

    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer seedToTrackerInterfaces;
    seedToTrackerInterfaces = address.Assign(seedToTrackerDevices);

    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer peerToTrackerInterfaces;
    peerToTrackerInterfaces = address.Assign(peerToTrackerDevices);


    // Add custom routing (we do not want packets to skip chokepoint via tracker node)
    NS_LOG_DEBUG("Add routing");
    Ptr<Ipv4> seedIpv4 = seeds.Get(0)->GetObject<Ipv4>(); // get the IPv4 instances
    Ptr<Ipv4> peerIpv4 = peers.Get(0)->GetObject<Ipv4>();
    Ptr<Ipv4> scIpv4 = chokers.Get(0)->GetObject<Ipv4>(); // seed choker
    Ptr<Ipv4> pcIpv4 = chokers.Get(1)->GetObject<Ipv4>(); // peer choker
    Ptr<Ipv4> trackerIpv4 = trackers.Get(0)->GetObject<Ipv4>();

    // Create static routing tables
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> seedStaticRoutingTable;
    seedStaticRoutingTable = ipv4RoutingHelper.GetStaticRouting(seedIpv4); // get routing table for seed
    // interface index 1 indexed (0 means loopback)
    seedStaticRoutingTable->AddHostRouteTo(Ipv4Address("10.1.2.1"), Ipv4Address("10.1.1.2"), 1); // to peer
    seedStaticRoutingTable->AddHostRouteTo(Ipv4Address("10.1.4.2"), Ipv4Address("10.1.4.2"), 2); // to tracker
    Ptr<Ipv4StaticRouting> peerStaticRoutingTable;
    peerStaticRoutingTable = ipv4RoutingHelper.GetStaticRouting(peerIpv4); // peer routing table
    peerStaticRoutingTable->AddHostRouteTo(Ipv4Address("10.1.1.1"), Ipv4Address("10.1.2.2"), 1); // to seed
    peerStaticRoutingTable->AddHostRouteTo(Ipv4Address("10.1.4.2"), Ipv4Address("10.1.5.2"), 2); // to tracker
    Ptr<Ipv4StaticRouting> scStaticRoutingTable;
    scStaticRoutingTable = ipv4RoutingHelper.GetStaticRouting(scIpv4); // seed choker routing table
    scStaticRoutingTable->AddHostRouteTo(Ipv4Address("10.1.1.1"), Ipv4Address("10.1.1.1"), 1); // to seed
    scStaticRoutingTable->AddHostRouteTo(Ipv4Address("10.1.2.1"), Ipv4Address("10.1.3.2"), 2); // to peer (via peer choker)
    Ptr<Ipv4StaticRouting> pcStaticRoutingTable;
    pcStaticRoutingTable = ipv4RoutingHelper.GetStaticRouting(pcIpv4); // peer choker routing table
    pcStaticRoutingTable->AddHostRouteTo(Ipv4Address("10.1.1.1"), Ipv4Address("10.1.3.1"), 2); // to seed (via seed choker)
    pcStaticRoutingTable->AddHostRouteTo(Ipv4Address("10.1.2.1"), Ipv4Address("10.1.2.1"), 1); // to peer
    Ptr<Ipv4StaticRouting> trackerStaticRoutingTable;
    trackerStaticRoutingTable = ipv4RoutingHelper.GetStaticRouting(trackerIpv4); // tracker routing table
    trackerStaticRoutingTable->AddHostRouteTo(Ipv4Address("10.1.1.1"), Ipv4Address("10.1.4.1"), 1); // to seed (via seed choker)
    trackerStaticRoutingTable->AddHostRouteTo(Ipv4Address("10.1.2.1"), Ipv4Address("10.1.5.1"), 2); // to peer

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

    // 1) Install a BitTorrentTracker application (with default values) on one of the nodes
    NS_LOG_DEBUG("(1) Add Tracker");
    Ptr<BitTorrentTracker> bitTorrentTracker = Create<BitTorrentTracker> ();
    trackers.Get (0)->AddApplication (bitTorrentTracker);

    // 2) Load a torrent file via the tracker application
    NS_LOG_DEBUG("(2) Load torrent file");
    Ptr<Torrent> sharedTorrent = bitTorrentTracker->AddTorrent ("input/bittorrent/torrent-data", "input/bittorrent/torrent-data/10MB-full.dat.torrent");

    // 3) Install BitTorrentClient applications on the desired number of nodes
    NS_LOG_DEBUG("(3a) Install BitTorrent Clients");
    ApplicationContainer bitTorrentClients;
    // Make this node a seeder (they must also have bittorrent client installed)
    Ptr<BitTorrentClient> client = Create<BitTorrentClient> ();
    client->SetTorrent (sharedTorrent);
    seeds.Get(0)->AddApplication (client);
    bitTorrentClients.Add(client);

    // Install client on all leechers
    client = Create<BitTorrentClient> ();
    client->SetTorrent (sharedTorrent);
    peers.Get(0)->AddApplication(client);
    bitTorrentClients.Add (client);

    NS_LOG_DEBUG("(3b) Mark Seeder");
    DynamicCast<BitTorrentClient> (seeds.Get(0)->GetApplication (0))->SetInitialBitfield ("full");

    // 4) Set up the BitTorrent metrics gatherer for output handling (here, we just log to the screen)
    NS_LOG_DEBUG("(4) Set up metrics");
    GlobalMetricsGatherer* gatherer = GlobalMetricsGatherer::GetInstance ();
    gatherer->SetFileNamePrefix ("This will be ignored while logging to the screen", false);
    gatherer->RegisterWithApplications (bitTorrentClients);
    gatherer->SetStopFraction (1.0, 1.0); // Stops the simulation when all nodes have finished downloading

    // 5a) Start the simulation
    NS_LOG_DEBUG("Starting Simulation...");
    bitTorrentTracker->SetStartTime(MilliSeconds(0));
    seeds.Get(0)->GetApplication(0)->SetStartTime(MilliSeconds(0));
    peers.Get(0)->GetApplication(0)->SetStartTime(MilliSeconds(1)); // start later or else 60s delay
    Simulator::Run ();
    Simulator::Destroy ();
    
    return 0;
}
