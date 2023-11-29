
/**
 * This file is a simple example of how to use the HELIX protocol. A client
 * and a server are set up, and the client sends the server one message at
 * 3 seconds, and the program ends at 20 seconds.
 * 
 * Network Topology:
 * 
 *                  5Mb/s, 2ms
 *      clientNode----------------serverNode
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


/**
 * \file
 *
 * This is a copy of the first example. It sets up a server and a client and
 * echos packets
 */

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("HelixExample");

/**
 * Close a socket
 *
 * \param socket The socket.
 */
void CloseSocket(Ptr<Socket> socket);

/**
 * Handles the read event. Passed as a callback invoked whenever a read occurs.
 * Prints out information corresponding to received packet.
 *
 * \param socket The socket.
 */
void HandleRead(Ptr<Socket> socket);

/**
 * Creates a Helix Socket and sets HandleRead as the recv callback function.
 *
 * \param socket The socket.
 */
void CreateServerHelixSocket(Ptr<Node> serverNode, const Address& serverAddress, const Address& clientAddress);

/**
 * Creates a helix socket and sends one message.
 *
 * \param socket The socket.
 */
void SendPacket(Ptr<Node> clientNode, const Address& serverAddress, const Address& clientAddress);


int
main(int argc, char* argv[])
{
    
    // Users may find it convenient to turn on explicit debugging
    // for selected modules; the below lines suggest how to do this
    //  LogComponentEnable("HelixpL4Protocol", LOG_LEVEL_ALL);
    //  LogComponentEnable("HelixSocketImpl", LOG_LEVEL_ALL);
    LogComponentEnable("HelixExample", LOG_ALL);

    // Setup and args
    // bool verbose = true;
    CommandLine cmd(__FILE__);
    // cmd.AddValue("verbose", "Tell application to log if true", verbose);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);

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
    HelixStackHelper helixStackHelper;
    helixStackHelper.AddHelix(clientNode);
    helixStackHelper.AddHelix(serverNode);

    // Create a set of IP addrs that these nodes can take on at p2p interface
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Addresses
    uint16_t port = 50000;
    Address clientAddress(InetSocketAddress(interfaces.GetAddress(0), port));
    Address serverAddress(InetSocketAddress(interfaces.GetAddress(1), port));

    // Start Server
    Simulator::Schedule(Seconds(1), &CreateServerHelixSocket, serverNode, serverAddress, clientAddress);

    // Schedule a send
    Simulator::Schedule(Seconds(3), &SendPacket, clientNode, serverAddress, clientAddress);

    Simulator::Stop(Seconds(20));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}


void
CloseSocket(Ptr<Socket> socket)
{
    socket->Close();
    socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
    socket = nullptr;
}


void
HandleRead(Ptr<Socket> socket)
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


void
CreateServerHelixSocket(Ptr<Node> serverNode, const Address& serverAddress, const Address& clientAddress)
{
    // Set up HELIX socket
    Ptr<SocketFactory> helixSocketFactory = serverNode->GetObject<HelixSocketFactoryImpl>();
    Ptr<Socket> socket = helixSocketFactory->CreateSocket();
    socket->Bind(serverAddress);
    socket->Connect(clientAddress);
    socket->SetRecvCallback(MakeCallback(&HandleRead));

    // Schedule socket close (is there a way to do this outside of this func?)
    Simulator::Schedule(Seconds(10), &CloseSocket, socket);
}


void
SendPacket(Ptr<Node> clientNode, const Address& serverAddress, const Address& clientAddress)
{
    // Create client HELIX connection socket
    Ptr<SocketFactory> helixSocketFactory = clientNode->GetObject<HelixSocketFactoryImpl>();
    Ptr<Socket> socket = helixSocketFactory->CreateSocket();
    socket->Bind(clientAddress);
    socket->Connect(serverAddress);
    socket->SetRecvCallback(MakeCallback(&HandleRead));

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
    Simulator::Schedule(Seconds(10), &CloseSocket, socket);
}