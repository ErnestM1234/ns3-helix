
// 1) try to implement UPD socket and send simple info
// 2) move to HELIX

#include "helix-socket.h"

#include "ns3/boolean.h"
#include "ns3/integer.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("HelixSocket");

NS_OBJECT_ENSURE_REGISTERED(HelixSocket);

TypeId
HelixSocket::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::HelixSocket")
            .SetParent<Socket>()
            .SetGroupName("Internet");
    return tid;
}

HelixSocket::HelixSocket()
{
    NS_LOG_FUNCTION(this);
}

HelixSocket::~HelixSocket()
{
    NS_LOG_FUNCTION(this);
}

} // namespace ns3




// Establishing a UDP socket
/*
    NS_LOG_FUNCTION(this << m_ipv4->GetAddress(i, 0).GetLocal());
    Ptr<Ipv4L3Protocol> l3 = m_ipv4->GetObject<Ipv4L3Protocol>();
    if (l3->GetNAddresses(i) > 1)
    {
        NS_LOG_WARN("AODV does not work with more then one address per each interface.");
    }
    Ipv4InterfaceAddress iface = l3->GetAddress(i, 0);
    if (iface.GetLocal() == Ipv4Address("127.0.0.1"))
    {
        return;
    }

    // Create a socket to listen only on this interface
    Ptr<Socket> socket = Socket::CreateSocket(GetObject<Node>(), HelixSocketFactory::GetTypeId());
    NS_ASSERT(socket);
    socket->SetRecvCallback(MakeCallback(&RoutingProtocol::RecvAodv, this));
    socket->BindToNetDevice(l3->GetNetDevice(i));
    socket->Bind(InetSocketAddress(iface.GetLocal(), AODV_PORT));
    socket->SetAllowBroadcast(true);
    socket->SetIpRecvTtl(true);
    m_socketAddresses.insert(std::make_pair(socket, iface));
*/