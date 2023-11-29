
/**
 * 
 * 
*/


#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"

// helix
#include "ns3/helix-helper.h"
#include "ns3/helix-socket.h"
#include "ns3/helix-l4-protocol.h"
#include "ns3/helix-socket-factory-impl.h"


// Helix rs implementation
// #include "/Users/ernestmccarter/Documents/Princeton/School/concentration/senior thesis/ns3/workspace/ns-allinone-3.40/helix-rs/helix_rs.h"




/**
 * \file
 *
 * This is a copy of the first example. It sets up a server and a client and
 * echos packets
 */

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("HelixExample");

// close a given socket
// TODO: move this to an example application
void
closeSocket(Ptr<Socket> socket)
{
    socket->Close();
    socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
    socket = nullptr;
}


// handle the reception of a packet
// TODO: move this to an example application
void
handleRead(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address from;
    while ((packet = socket->RecvFrom(from)))
    {
        if (InetSocketAddress::IsMatchingType(from))
        {
            NS_LOG_INFO("At time " << Simulator::Now().As(Time::S) << " server received "
                                   << packet->GetSize() << " bytes from "
                                   << InetSocketAddress::ConvertFrom(from).GetIpv4() << " port "
                                   << InetSocketAddress::ConvertFrom(from).GetPort());
        }
        else if (Inet6SocketAddress::IsMatchingType(from))
        {
            NS_LOG_INFO("At time " << Simulator::Now().As(Time::S) << " server received "
                                   << packet->GetSize() << " bytes from "
                                   << Inet6SocketAddress::ConvertFrom(from).GetIpv6() << " port "
                                   << Inet6SocketAddress::ConvertFrom(from).GetPort());
        } else {
            NS_LOG_INFO("At time " << Simulator::Now().As(Time::S) << " server received "
                                   << packet->GetSize() << " bytes.");
        }
    }
}




// set up a helix socket on the server node to receive packets
// TODO: move this to an example application
void
createServerHelixSocket(Ptr<Node> serverNode, const Address& serverAddress, const Address& clientAddress)
{
    // Set up HELIX socket
    Ptr<SocketFactory> helixSocketFactory = serverNode->GetObject<HelixSocketFactoryImpl>();
    Ptr<Socket> socket = helixSocketFactory->CreateSocket();
    socket->Bind(serverAddress);
    socket->Connect(clientAddress);
    socket->SetRecvCallback(MakeCallback(&handleRead));

    // Schedule socket close (is there a way to do this outside of this func?)
    Simulator::Schedule(Seconds(10), &closeSocket, socket);
}

// set up a helix socket on the client node and send to the server address
// TODO: move this to an example application
void
sendPacket(Ptr<Node> clientNode, const Address& serverAddress, const Address& clientAddress)
{
    // Create client HELIX connection socket
    Ptr<SocketFactory> helixSocketFactory = clientNode->GetObject<HelixSocketFactoryImpl>();
    Ptr<Socket> socket = helixSocketFactory->CreateSocket();
    socket->Bind(clientAddress);
    socket->Connect(serverAddress);
    socket->SetRecvCallback(MakeCallback(&handleRead));

    // generate a packet to send
    uint16_t pktSize = 10;
    Ptr<Packet> packet;
    packet = Create<Packet>(pktSize);

    // attempt to send packet
    int actual = socket->Send(packet);
    if ((unsigned)actual == pktSize)
    {
        NS_LOG_INFO("At time " << Simulator::Now().As(Time::S) << " client application sent "
                                << packet->GetSize() << " bytes to "
                                << InetSocketAddress::ConvertFrom(serverAddress).GetIpv4() << " port "
                                << InetSocketAddress::ConvertFrom(serverAddress).GetPort()
                                << " total Tx " << pktSize << " bytes");
    }
    else
    {
        NS_LOG_DEBUG("Unable to send packet; actual " << actual << " size " << pktSize
                                                      << "; not caching for later attempt");
    }
    Simulator::Schedule(Seconds(10), &closeSocket, socket);
}


int
main(int argc, char* argv[])
{
    
    LogComponentEnable("HelixExample", LOG_ALL);
    // LogComponentEnable("HelixSocketImpl", LOG_ALL);
    // LogComponentEnable("Socket", LOG_ALL);
    // Setup and args
    // bool verbose = true;
    CommandLine cmd(__FILE__);
    // cmd.AddValue("verbose", "Tell application to log if true", verbose);
    cmd.Parse(argc, argv);


    Time::SetResolution(Time::NS);
    // LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    // LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
    // LogComponentEnable("HelixExample", LOG_LEVEL_INFO);

    // Create two nodes and a NodeContainer for node management
    NodeContainer nodes;
    nodes.Create(2);

    // Label our nodes
    Ptr<Node> clientNode = nodes.Get(0);
    Ptr<Node> serverNode = nodes.Get(1); // may have to come back and check this

    // Create a point to point connection
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    // Add the previous nodes as the nodes in the point to point connection
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    // Add internet stack (this will not aggregate helix with node by default so we have to do it ourselves)
    InternetStackHelper stack;
    stack.Install(nodes);

    // Aggregate helix onto nodes
    Ptr<HelixL4Protocol> helix = CreateObject<HelixL4Protocol>();
    Ptr<HelixL4Protocol> helix2 = CreateObject<HelixL4Protocol>();
    HelixStackHelper helixStackHelper;
    helixStackHelper.AddHelix(clientNode, helix); // TODO: figure out why you only have to do aggregation on one node?
    helixStackHelper.AddHelix(serverNode, helix2);

    // Create a set of IP addrs that these nodes can take on at p2p interface
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Addresses
    uint16_t port = 50000;
    Address clientAddress(InetSocketAddress(interfaces.GetAddress(0), port));
    Address serverAddress(InetSocketAddress(interfaces.GetAddress(1), port));

    // Create a packet sink on the receiver
    // PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", serverAddress); // able to integrate with our Helix built on UDP?
    // ApplicationContainer sinkApp = sinkHelper.Install(serverNode);
    // sinkApp.Start(Seconds(1.0));
    // sinkApp.Stop(Seconds(10.0));

    // Ptr<SocketFactory> helixSocketFactory = serverNode->GetObject<HelixSocketFactoryImpl>();
    // Ptr<Socket> s_socket = helixSocketFactory->CreateSocket();
    // s_socket->Bind(serverAddress);
    // s_socket->Connect(serverAddress);
    // s_socket->SetRecvCallback(MakeCallback(&handleRead));



    // Create the OnOff applications to send data to the UDP receiver
    // OnOffHelper clientHelper("ns3::UdpSocketFactory", Address());
    // AddressValue remoteAddress(InetSocketAddress(interfaces.GetAddress(1), port));
    // clientHelper.SetAttribute("Remote", remoteAddress);
    // ApplicationContainer clientApps = (clientHelper.Install(clientNode));
    // clientApps.Start(Seconds(2.0));
    // clientApps.Stop(Seconds(9.0));




    // Start Server
    Simulator::Schedule(Seconds(1), &createServerHelixSocket, serverNode, serverAddress, clientAddress);

    // Schedule a send
    Simulator::Schedule(Seconds(3), &sendPacket, clientNode, serverAddress, clientAddress);

    Simulator::Stop(Seconds(20));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
