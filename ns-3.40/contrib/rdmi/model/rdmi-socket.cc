
// RDMI
#include "rdmi-socket.h"

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(RDMISocket);
NS_LOG_COMPONENT_DEFINE("RDMI_Socket");

TypeId RDMISocket::GetTypeId()
{
  static TypeId tid =
    TypeId("ns3::RDMI")
        .SetParent<Socket>()
        .AddAttribute("MaxSegLifetime",
                        "Maximum segment lifetime in seconds, use for TIME_WAIT state transition "
                        "to CLOSED state",
                        DoubleValue(120), /* RFC793 says MSL=2 minutes*/
                        MakeDoubleAccessor(&TcpSocketBase::m_msl),
                        MakeDoubleChecker<double>(0))
    ;
  return tid;
}

RDMISocket::RDMISocket()
{
}

RDMISocket::~RDMISocket()
{
}

void 
RDMISocket::SetRdmi(Ptr<RDMI> rdmi)
{
    NS_LOG_FUNCTION(this << rdmi);
    m_rdmi_client = rdmi;
}

void 
RDMISocket::SetPeer(Ptr<RDMIPeer> peer)
{
    NS_LOG_FUNCTION(this << peer);
    m_rdmi_peer = peer;
}

uint16_t
RDMISocket::GetDestinationPort() const
{
    NS_LOG_FUNCTION(this);
    return m_dest_port;
}

uint16_t
RDMISocket::GetPort() const
{
    NS_LOG_FUNCTION(this);
    return m_port;
}

Socket::SocketErrno
RDMISocket::GetErrno() const
{
    NS_LOG_FUNCTION(this);
    return m_errno;
}

Socket::SocketType
RDMISocket::GetSocketType() const
{
    return NS3_SOCK_DGRAM;
}

Ptr<Node>
RDMISocket::GetNode() const
{
    NS_LOG_FUNCTION(this);
    // this will be null because this is a virtual socket
    return m_node;
}

int
RDMISocket::Bind()
{
    NS_LOG_FUNCTION(this);
    return -1;
}

int
RDMISocket::Bind6()
{
    NS_LOG_FUNCTION(this);
    return -1;
}

int
RDMISocket::Bind(const Address& address)
{
    NS_LOG_FUNCTION(this << address);
    return -1;
}

int
RDMISocket::Close()
{
    NS_LOG_FUNCTION(this);
    return -1;
}

int
RDMISocket::ShutdownSend()
{
    NS_LOG_FUNCTION(this);
    return -1;
}

int
RDMISocket::ShutdownRecv()
{
    NS_LOG_FUNCTION(this);
    return -1;
}

int
RDMISocket::Connect(const Address& address)
{
    NS_LOG_FUNCTION(this << address);
    return -1;
}

int
RDMISocket::Listen()
{
    NS_LOG_FUNCTION(this);
    return -1;
}

uint32_t
RDMISocket::GetTxAvailable() const
{
    NS_LOG_FUNCTION(this);
    return m_rdmi_peer->GetTxAvailable(m_port);
}

int
RDMISocket::Send(Ptr<Packet> p, uint32_t flags)
{
    NS_LOG_FUNCTION(this << p << flags);
    return m_rdmi_peer->Send(m_port, p, flags);
}


int
RDMISocket::SendTo(Ptr<Packet> p, uint32_t flags, const Address& address)
{
    NS_LOG_FUNCTION(this << p << flags);
    // we do not support sending to a specific address
    return this->Send(p, flags);
}

uint32_t
RDMISocket::GetRxAvailable() const
{
    NS_LOG_FUNCTION(this);
    return m_rdmi_peer->GetRxAvailable(m_port);
}

Ptr<Packet>
RDMISocket::Recv(uint32_t maxSize, uint32_t flags)
{
    NS_LOG_FUNCTION(this << maxSize << flags);
    return m_rdmi_peer->Recv(m_port, maxSize, flags);
}


Ptr<Packet>
RDMISocket::RecvFrom(uint32_t maxSize, uint32_t flags, Address& fromAddress)
{
    NS_LOG_FUNCTION(this << maxSize << flags);
    // this is not supported, just call recv
    return m_rdmi_peer->Recv(m_port, maxSize, flags);
}

int
RDMISocket::GetSockName(Address& address) const
{
    NS_LOG_FUNCTION(this);
    // TODO: Not supported
    return -1;
}


int
RDMISocket::GetPeerName(Address& address) const
{
    NS_LOG_FUNCTION(this);
    // TODO: Not supported
    return -1;
}

void
RDMISocket::BindToNetDevice(Ptr<NetDevice> netdevice)
{
    NS_LOG_FUNCTION(this << netdevice);
    // nothing needs to happen here because this is a virtual bind. Actual
    // binding happens in the Reliable Datagram Delvery (RDD) protocol
    return;
}

bool
RDMISocket::SetAllowBroadcast(bool allowBroadcast)
{
    // Broadcast is not implemented. Return true only if allowBroadcast==false
    return (!allowBroadcast);
}

bool
RDMISocket::GetAllowBroadcast() const
{
    // Broadcast not implemented
    return false;
}

void
RDMISocket::Ipv6JoinGroup
(
    Ipv6Address address,
    Socket::Ipv6MulticastFilterMode filterMode,
    std::vector<Ipv6Address> sourceAddresses
)
{
    // Not implemented
    return;
}


} // namespace ns3
