/*
 * Give helix access to callbacks for sending info?
 */

#include "helix-socket-impl.h"
#include "helix-rs-interface.h"

#include "ns3/udp-socket.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/node.h"
#include "ns3/ptr.h"
#include "ns3/callback.h"
#include <limits>


namespace ns3
{

// Strategy: instantiate HelixSocketImpl object and interface with that

NS_LOG_COMPONENT_DEFINE("HelixSocketImpl");

NS_OBJECT_ENSURE_REGISTERED(HelixSocketImpl);

// Add attributes generic to all UdpSockets to base class UdpSocket
TypeId
HelixSocketImpl::GetTypeId()
{
    static TypeId tid = TypeId("ns3::HelixSocketImpl")
                                                .SetParent<HelixSocket>()
                                                .SetGroupName("Internet")
                                                .AddConstructor<HelixSocketImpl>();
    return tid;
}

HelixSocketImpl::HelixSocketImpl()
    : m_node(nullptr),
      m_udp_socket(nullptr),
      m_helix(nullptr),
      m_helix_rs_interface(nullptr)
{
    NS_LOG_FUNCTION(this);
    m_helix_rs_interface = CreateObject<HelixRsInterface>(); // TODO: Use attribute system instead
}

HelixSocketImpl::~HelixSocketImpl()
{
    NS_LOG_FUNCTION(this);

    // if (m_helix_rs_interface) {
    //     m_helix_rs_interface->~HelixRsInterface();
    // }
    m_helix_rs_interface = nullptr;

    // if (m_udp_socket != nullptr) {
    //     delete m_udp_socket;
    //     m_udp_socket = nullptr;
    // }

    m_node = nullptr;
}

void
HelixSocketImpl::SetNode(Ptr<Node> node)
{
    NS_LOG_FUNCTION(this << node);
    m_node = node;
    // No need to pass node to m_udp_socket, this is done already when it was initialized in udp-l4-protocol
}

void
HelixSocketImpl::SetHelix(Ptr<HelixL4Protocol> helix)
{
    NS_LOG_FUNCTION(this << helix);
    m_helix = helix;
}

void
HelixSocketImpl::SetUdpSocket(Ptr<Socket> udp_socket)
{
    NS_LOG_FUNCTION(this << udp_socket);
    m_udp_socket = udp_socket;
}

Socket::SocketErrno
HelixSocketImpl::GetErrno() const
{
    NS_LOG_FUNCTION(this);
    return m_udp_socket->GetErrno();
}

Socket::SocketType
HelixSocketImpl::GetSocketType() const
{
    return m_udp_socket->GetSocketType();
}

Ptr<Node>
HelixSocketImpl::GetNode() const
{
    NS_LOG_FUNCTION(this);
    return m_udp_socket->GetNode();
}

void 
HelixSocketImpl::SetRecvCallback(Callback<void, Ptr<Socket>> receivedData)
{
    NS_LOG_FUNCTION(this);
    m_handle_recv = receivedData;

    // tie udp recv callback to trigger a helix function instead
    m_udp_socket->SetRecvCallback(MakeCallback(&HelixSocketImpl::HandleRecv, this));
}

void
HelixSocketImpl::SetSendCallback(Callback<void, Ptr<Socket>, uint32_t> sendCb)
{
    NS_LOG_FUNCTION(this);
    m_handle_send = sendCb;

    // tie udp send callback to trigger a helix function instead
    m_udp_socket->SetSendCallback(MakeCallback(&HelixSocketImpl::HandleSend, this));
}

void
HelixSocketImpl::HandleRecv(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this);

    // pass a reference to this socket instead of udp socket
    // this is because these functions will invoke udp functions
    // in a 1-to-1 mapping
    m_handle_recv(this);
}

void
HelixSocketImpl::HandleSend(Ptr<Socket> socket, uint32_t num_bytes)
{
    NS_LOG_FUNCTION(this);

    // pass a reference to this socket instead of udp socket
    // this is because these functions will invoke udp functions
    // in a 1-to-1 mapping
    m_handle_send(this, num_bytes);
}

void
HelixSocketImpl::BindToNetDevice(Ptr<NetDevice> netdevice)
{
    NS_LOG_FUNCTION(netdevice);

    // const Address& address = netdevice->GetAddress();
    Address address = Address();
    m_helix_rs_interface->Bind(address);

    m_udp_socket->BindToNetDevice(netdevice);
}

int
HelixSocketImpl::Bind()
{
    NS_LOG_FUNCTION(this);

    // TODO: rust will make a callback to bind
    // TODO: provide address
    Address address = Address();
    m_helix_rs_interface->Bind(address);

    return m_udp_socket->Bind();
}

int
HelixSocketImpl::Bind6()
{
    NS_LOG_FUNCTION(this);

    // TODO: rust will make a callback to bind
    // TODO: provide address
    Address address = Address();
    m_helix_rs_interface->Bind(address);

    return m_udp_socket->Bind6();
}

int
HelixSocketImpl::Bind(const Address& address)
{
    NS_LOG_FUNCTION(this << address);

    // TODO: rust will make a callback to bind
    Address addr = Address();
    m_helix_rs_interface->Bind(addr);

    return m_udp_socket->Bind(address);
}

int
HelixSocketImpl::Connect(const Address& address)
{
    NS_LOG_FUNCTION(this << address);


    // TODO: rust will make a callback to connect
    Address addr = Address();
    m_helix_rs_interface->Connect(addr);

    return m_udp_socket->Connect(address);
}

int
HelixSocketImpl::Listen()
{
    // TODO: rust will make a callback to listen
    m_helix_rs_interface->Listen();

    return m_udp_socket->Listen();
}

int
HelixSocketImpl::Send(Ptr<Packet> p, uint32_t flags)
{
    NS_LOG_FUNCTION(this << p << flags);

    // TODO: Add logic for adding flags
    // TODO: remove m_udp_socket->Send(p, flags), because this will be called by rust
    m_helix_rs_interface->Send(p);

    return m_udp_socket->Send(p, flags);
}


int
HelixSocketImpl::SendTo(Ptr<Packet> p, uint32_t flags, const Address& address)
{
    NS_LOG_FUNCTION(this << p << flags << address);


    // TODO: Add logic for adding address
    // TODO: Add logic for adding flags
    // TODO: remove m_udp_socket->Send(p, flags), because this will be called by rust
    m_helix_rs_interface->Send(p);

    // TODO: Call Helix rs packet encoder implementation
    return m_udp_socket->SendTo(p, flags, address);
}

Ptr<Packet>
HelixSocketImpl::Recv(uint32_t maxSize, uint32_t flags)
{
    NS_LOG_FUNCTION(this << maxSize << flags);

    Ptr<Packet> p = m_udp_socket->Recv(maxSize, flags);
    return m_helix_rs_interface->Recv(p);
}

Ptr<Packet>
HelixSocketImpl::RecvFrom(uint32_t maxSize, uint32_t flags, Address& fromAddress)
{
    NS_LOG_FUNCTION(this << maxSize << flags << fromAddress);
    
    // TODO: Add address
    // TODO: pass received info to m_helix_rs_interface
    Ptr<Packet> p = m_udp_socket->RecvFrom(maxSize, flags, fromAddress);
    return m_helix_rs_interface->Recv(p);
}

int
HelixSocketImpl::Close()
{
    NS_LOG_FUNCTION(this);

    // TODO: rust will make a callback to udp close
    m_helix_rs_interface->Close();

    return m_udp_socket->Close();
}

int
HelixSocketImpl::ShutdownSend()
{
    NS_LOG_FUNCTION(this);
    return m_udp_socket->ShutdownSend();
}

int
HelixSocketImpl::ShutdownRecv()
{
    NS_LOG_FUNCTION(this);
    return m_udp_socket->ShutdownRecv();
}

uint32_t
HelixSocketImpl::GetTxAvailable() const
{
    NS_LOG_FUNCTION(this);
    return m_udp_socket->GetTxAvailable();
}

uint32_t
HelixSocketImpl::GetRxAvailable() const
{
    NS_LOG_FUNCTION(this);
    return m_udp_socket->GetRxAvailable();
}

int
HelixSocketImpl::GetSockName(Address& address) const
{
    NS_LOG_FUNCTION(this << address);
    return m_udp_socket->GetSockName(address);
}

int
HelixSocketImpl::GetPeerName(Address& address) const
{
    NS_LOG_FUNCTION(this << address);
    return m_udp_socket->GetPeerName(address);
}

bool
HelixSocketImpl::SetAllowBroadcast(bool allowBroadcast)
{
    return m_udp_socket->SetAllowBroadcast(allowBroadcast);
}

bool
HelixSocketImpl::GetAllowBroadcast() const
{
    return m_udp_socket->GetAllowBroadcast();
}

void
HelixSocketImpl::Ipv6JoinGroup(Ipv6Address address,
                             Socket::Ipv6MulticastFilterMode filterMode,
                             std::vector<Ipv6Address> sourceAddresses)
{
    NS_LOG_FUNCTION(this << address << &filterMode << &sourceAddresses);
    m_udp_socket->Ipv6JoinGroup(address, filterMode, sourceAddresses);
}



} // namespace ns3
