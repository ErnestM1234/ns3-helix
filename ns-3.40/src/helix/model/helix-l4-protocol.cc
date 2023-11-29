

#include "helix-l4-protocol.h"
#include "helix-socket-factory-impl.h"
#include "helix-socket-impl.h"

#include "ns3/assert.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/object-map.h"
#include "ns3/packet.h"
#include "ns3/callback.h"

#include <unordered_map>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("HelixL4Protocol");

NS_OBJECT_ENSURE_REGISTERED(HelixL4Protocol);


/* see http://www.iana.org/assignments/protocol-numbers */
const uint8_t HelixL4Protocol::PROT_NUMBER = 253; // designated for testing purposes

TypeId
HelixL4Protocol::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::HelixL4Protocol")
            .SetParent<IpL4Protocol>()
            .SetGroupName("Internet")
            .AddConstructor<HelixL4Protocol>()
            .AddAttribute("SocketList",
                          "A container of sockets associated to this protocol. "
                          "The underlying type is an unordered map, the attribute name "
                          "is kept for backward compatibility.",
                          ObjectMapValue(),
                          MakeObjectMapAccessor(&HelixL4Protocol::m_sockets),
                          MakeObjectMapChecker<HelixSocketImpl>());
    return tid;
}

HelixL4Protocol::HelixL4Protocol()
    : m_node(nullptr)
{
    NS_LOG_FUNCTION(this);
}

HelixL4Protocol::~HelixL4Protocol()
{
    NS_LOG_FUNCTION(this);
}

void
HelixL4Protocol::SetNode(Ptr<Node> node)
{
    m_node = node;
}

int
HelixL4Protocol::GetProtocolNumber() const
{
    return PROT_NUMBER;
}


Ptr<Socket>
HelixL4Protocol::CreateSocket()
{
    NS_LOG_FUNCTION(this);
    // create socket
    Ptr<HelixSocketImpl> socket = CreateObject<HelixSocketImpl>();

    // set members
    socket->SetNode(m_node);
    socket->SetHelix(this);

    // create a udp socket
    Ptr<SocketFactory> udpSocketFactory = m_node->GetObject<UdpSocketFactory>(); // how to get access to the node?
    Ptr<Socket> udp_socket = udpSocketFactory->CreateSocket();
    socket->SetUdpSocket(udp_socket);

    // internal socket tracking
    m_sockets[m_socketIndex++] = socket;

    return socket;
}





void
HelixL4Protocol::ReceiveIcmp(Ipv4Address icmpSource,
                           uint8_t icmpTtl,
                           uint8_t icmpType,
                           uint8_t icmpCode,
                           uint32_t icmpInfo,
                           Ipv4Address payloadSource,
                           Ipv4Address payloadDestination,
                           const uint8_t payload[8])
{
    NS_LOG_FUNCTION(this << icmpSource << icmpTtl << icmpType << icmpCode << icmpInfo
                         << payloadSource << payloadDestination);
}

void
HelixL4Protocol::ReceiveIcmp(Ipv6Address icmpSource,
                           uint8_t icmpTtl,
                           uint8_t icmpType,
                           uint8_t icmpCode,
                           uint32_t icmpInfo,
                           Ipv6Address payloadSource,
                           Ipv6Address payloadDestination,
                           const uint8_t payload[8])
{
    NS_LOG_FUNCTION(this << icmpSource << icmpTtl << icmpType << icmpCode << icmpInfo
                         << payloadSource << payloadDestination);
}

IpL4Protocol::RxStatus
HelixL4Protocol::Receive(Ptr<Packet> packet, const Ipv4Header& header, Ptr<Ipv4Interface> interface)
{
    NS_LOG_FUNCTION(this << packet << header);
    return IpL4Protocol::RX_OK;
}

IpL4Protocol::RxStatus
HelixL4Protocol::Receive(Ptr<Packet> packet, const Ipv6Header& header, Ptr<Ipv6Interface> interface)
{
    NS_LOG_FUNCTION(this << packet << header.GetSource() << header.GetDestination());
    return IpL4Protocol::RX_OK;
}

void
HelixL4Protocol::SetDownTarget(IpL4Protocol::DownTargetCallback callback)
{
    NS_LOG_FUNCTION(this);
    m_downTarget = callback;
}

IpL4Protocol::DownTargetCallback
HelixL4Protocol::GetDownTarget() const
{
    return m_downTarget;
}

void
HelixL4Protocol::SetDownTarget6(IpL4Protocol::DownTargetCallback6 callback)
{
    NS_LOG_FUNCTION(this);
    m_downTarget6 = callback;
}

IpL4Protocol::DownTargetCallback6
HelixL4Protocol::GetDownTarget6() const
{
    return m_downTarget6;
}


void
HelixL4Protocol::DoDispose()
{
    NS_LOG_FUNCTION(this);
    for (auto i = m_sockets.begin(); i != m_sockets.end(); i++)
    {
        i->second = nullptr;
    }
    m_sockets.clear();

    m_node = nullptr;

    IpL4Protocol::DoDispose();
}

/*
 * This method is called by AggregateObject and completes the aggregation
 * by setting the node in the udp stack and link it to the ipv4 object
 * present in the node along with the socket factory
 */
void
HelixL4Protocol::NotifyNewAggregate()
{
    NS_LOG_FUNCTION(this);
    // m_udp->NotifyNewAggregate();
    // Ptr<Node> node = this->GetObject<Node>();
    // Ptr<Ipv4> ipv4 = this->GetObject<Ipv4>();
    // Ptr<Ipv6> ipv6 = node->GetObject<Ipv6>();
    // Ptr<UdpL4Protocol> udp = node->GetObject<UdpL4Protocol>();

    // // Set node

    // if (!m_node)
    // {
    //     if (node && udp)
    //     {
    //         this->SetNode(node);
    //         Ptr<HelixSocketFactoryImpl> helixFactory = CreateObject<HelixSocketFactoryImpl>();
    //         helixFactory->SetHelix(this);
    //         node->AggregateObject(helixFactory);
    //     }
    // }

    // // Set down target
    // if (udp)
    // {
    //     this->SetDownTarget(MakeCallback(&UdpL4Protocol::Send, udp));
    // }
    // udp->AggregateObject(this);




    // Ptr<Node> node = this->GetObject<Node>();
    // Ptr<Ipv4> ipv4 = this->GetObject<Ipv4>();
    // Ptr<Ipv6> ipv6 = node->GetObject<Ipv6>();


    // if (!m_node)
    // {
    //     if (node && (ipv4 || ipv6))
    //     {
    //         this->SetNode(node);
    //         Ptr<HelixSocketFactoryImpl> helixFactory = CreateObject<HelixSocketFactoryImpl>();
    //         helixFactory->SetHelix(this);
    //         node->AggregateObject(helixFactory);
    //     }
    // }

    // // We set at least one of our 2 down targets to the IPv4/IPv6 send
    // // functions.  Since these functions have different prototypes, we
    // // need to keep track of whether we are connected to an IPv4 or
    // // IPv6 lower layer and call the appropriate one.

    // if (ipv4 && m_downTarget.IsNull())
    // {
    //     ipv4->Insert(this);
    //     this->SetDownTarget(MakeCallback(&Ipv4::Send, ipv4));
    // }
    // if (ipv6 && m_downTarget6.IsNull())
    // {
    //     ipv6->Insert(this);
    //     this->SetDownTarget6(MakeCallback(&Ipv6::Send, ipv6));
    // }
    // IpL4Protocol::NotifyNewAggregate();
}


} // namespace ns3
