

#include "helix-l4-protocol.h"
#include "helix-socket-factory-impl.h"
#include "helix-socket-impl.h"

#include "ns3/assert.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/object-map.h"
#include "ns3/packet.h"

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
    // uint16_t src;
    // uint16_t dst;
    // src = payload[0] << 8;
    // src |= payload[1];
    // dst = payload[2] << 8;
    // dst |= payload[3];

    // Ipv4EndPoint* endPoint = m_endPoints->SimpleLookup(payloadSource, src, payloadDestination, dst);
    // if (endPoint != nullptr)
    // {
    //     endPoint->ForwardIcmp(icmpSource, icmpTtl, icmpType, icmpCode, icmpInfo);
    // }
    // else
    // {
    //     NS_LOG_DEBUG("no endpoint found source=" << payloadSource
    //                                              << ", destination=" << payloadDestination
    //                                              << ", src=" << src << ", dst=" << dst);
    // }
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
    // uint16_t src;
    // uint16_t dst;
    // src = payload[0] << 8;
    // src |= payload[1];
    // dst = payload[2] << 8;
    // dst |= payload[3];

    // Ipv6EndPoint* endPoint =
    //     m_endPoints6->SimpleLookup(payloadSource, src, payloadDestination, dst);
    // if (endPoint != nullptr)
    // {
    //     endPoint->ForwardIcmp(icmpSource, icmpTtl, icmpType, icmpCode, icmpInfo);
    // }
    // else
    // {
    //     NS_LOG_DEBUG("no endpoint found source=" << payloadSource
    //                                              << ", destination=" << payloadDestination
    //                                              << ", src=" << src << ", dst=" << dst);
    // }
}

IpL4Protocol::RxStatus
HelixL4Protocol::Receive(Ptr<Packet> packet, const Ipv4Header& header, Ptr<Ipv4Interface> interface)
{
    NS_LOG_FUNCTION(this << packet << header);
    // UdpHeader udpHeader;
    // if (Node::ChecksumEnabled())
    // {
    //     udpHeader.EnableChecksums();
    // }

    // udpHeader.InitializeChecksum(header.GetSource(), header.GetDestination(), PROT_NUMBER);

    // // We only peek at the header for now (instead of removing it) so that it will be intact
    // // if we have to pass it to a IPv6 endpoint via:
    // //
    // //   HelixL4Protocol::Receive (Ptr<Packet> packet, Ipv6Address &src, Ipv6Address &dst, ...)

    // packet->PeekHeader(udpHeader);

    // if (!udpHeader.IsChecksumOk())
    // {
    //     NS_LOG_INFO("Bad checksum : dropping packet!");
    //     return IpL4Protocol::RX_CSUM_FAILED;
    // }

    // NS_LOG_DEBUG("Looking up dst " << header.GetDestination() << " port "
    //                                << udpHeader.GetDestinationPort());
    // Ipv4EndPointDemux::EndPoints endPoints = m_endPoints->Lookup(header.GetDestination(),
    //                                                              udpHeader.GetDestinationPort(),
    //                                                              header.GetSource(),
    //                                                              udpHeader.GetSourcePort(),
    //                                                              interface);
    // if (endPoints.empty())
    // {
    //     if (this->GetObject<Ipv6>())
    //     {
    //         NS_LOG_LOGIC("  No Ipv4 endpoints matched on HelixL4Protocol, trying Ipv6 " << this);
    //         Ptr<Ipv6Interface> fakeInterface;
    //         Ipv6Header ipv6Header;
    //         Ipv6Address src = Ipv6Address::MakeIpv4MappedAddress(header.GetSource());
    //         Ipv6Address dst = Ipv6Address::MakeIpv4MappedAddress(header.GetDestination());
    //         ipv6Header.SetSource(src);
    //         ipv6Header.SetDestination(dst);
    //         return (this->Receive(packet, ipv6Header, fakeInterface));
    //     }

    //     NS_LOG_LOGIC("RX_ENDPOINT_UNREACH");
    //     return IpL4Protocol::RX_ENDPOINT_UNREACH;
    // }

    // packet->RemoveHeader(udpHeader);
    // for (auto endPoint = endPoints.begin(); endPoint != endPoints.end(); endPoint++)
    // {
    //     (*endPoint)->ForwardUp(packet->Copy(), header, udpHeader.GetSourcePort(), interface);
    // }
    return IpL4Protocol::RX_OK;
}

IpL4Protocol::RxStatus
HelixL4Protocol::Receive(Ptr<Packet> packet, const Ipv6Header& header, Ptr<Ipv6Interface> interface)
{
    NS_LOG_FUNCTION(this << packet << header.GetSource() << header.GetDestination());
    // UdpHeader udpHeader;
    // if (Node::ChecksumEnabled())
    // {
    //     udpHeader.EnableChecksums();
    // }

    // udpHeader.InitializeChecksum(header.GetSource(), header.GetDestination(), PROT_NUMBER);

    // packet->RemoveHeader(udpHeader);

    // if (!udpHeader.IsChecksumOk() && !header.GetSource().IsIpv4MappedAddress())
    // {
    //     NS_LOG_INFO("Bad checksum : dropping packet!");
    //     return IpL4Protocol::RX_CSUM_FAILED;
    // }

    // NS_LOG_DEBUG("Looking up dst " << header.GetDestination() << " port "
    //                                << udpHeader.GetDestinationPort());
    // Ipv6EndPointDemux::EndPoints endPoints = m_endPoints6->Lookup(header.GetDestination(),
    //                                                               udpHeader.GetDestinationPort(),
    //                                                               header.GetSource(),
    //                                                               udpHeader.GetSourcePort(),
    //                                                               interface);
    // if (endPoints.empty())
    // {
    //     NS_LOG_LOGIC("RX_ENDPOINT_UNREACH");
    //     return IpL4Protocol::RX_ENDPOINT_UNREACH;
    // }
    // for (auto endPoint = endPoints.begin(); endPoint != endPoints.end(); endPoint++)
    // {
    //     (*endPoint)->ForwardUp(packet->Copy(), header, udpHeader.GetSourcePort(), interface);
    // }
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
    /*
     = MakeNullCallback<void,Ptr<Packet>, Ipv4Address, Ipv4Address, uint8_t, Ptr<Ipv4Route> > ();
    */
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
    Ptr<Node> node = this->GetObject<Node>();
    Ptr<Ipv4> ipv4 = this->GetObject<Ipv4>();
    Ptr<Ipv6> ipv6 = node->GetObject<Ipv6>();

    if (!m_node)
    {
        if (node && (ipv4 || ipv6))
        {
            this->SetNode(node);
            Ptr<HelixSocketFactoryImpl> helixFactory = CreateObject<HelixSocketFactoryImpl>();
            helixFactory->SetHelix(this);
            node->AggregateObject(helixFactory);
        }
    }

    // We set at least one of our 2 down targets to the IPv4/IPv6 send
    // functions.  Since these functions have different prototypes, we
    // need to keep track of whether we are connected to an IPv4 or
    // IPv6 lower layer and call the appropriate one.

    if (ipv4 && m_downTarget.IsNull())
    {
        ipv4->Insert(this);
        this->SetDownTarget(MakeCallback(&Ipv4::Send, ipv4));
    }
    if (ipv6 && m_downTarget6.IsNull())
    {
        ipv6->Insert(this);
        this->SetDownTarget6(MakeCallback(&Ipv6::Send, ipv6));
    }
    IpL4Protocol::NotifyNewAggregate();
}


} // namespace ns3
