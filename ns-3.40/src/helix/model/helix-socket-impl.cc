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
      m_udpSocket(nullptr),
      m_helix(nullptr),
      m_helix_rs_interface(nullptr)
{
    NS_LOG_FUNCTION(this);
    m_helix_rs_interface = CreateObject<HelixRsInterface>(); // TODO: Use attribute system instead
}

HelixSocketImpl::~HelixSocketImpl()
{
    NS_LOG_FUNCTION(this);
}

void
HelixSocketImpl::SetNode(Ptr<Node> node)
{
    NS_LOG_FUNCTION(this << node);
    m_node = node;
    // No need to pass node to m_udpSocket, this is done already when it was initialized in udp-l4-protocol
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
    m_udpSocket = udp_socket;
}

Socket::SocketErrno
HelixSocketImpl::GetErrno() const
{
    NS_LOG_FUNCTION(this);
    return m_udpSocket->GetErrno();
}

Socket::SocketType
HelixSocketImpl::GetSocketType() const
{
    return m_udpSocket->GetSocketType();
}

Ptr<Node>
HelixSocketImpl::GetNode() const
{
    NS_LOG_FUNCTION(this);
    return m_udpSocket->GetNode();
}

void
HelixSocketImpl::SetConnectCallback(
    Callback<void, Ptr<Socket>> connectionSucceeded,
    Callback<void, Ptr<Socket>> connectionFailed)
{
    NS_LOG_FUNCTION(this);

    m_handleConnectionSucceeded = connectionSucceeded;
    m_handleConnectionFailed = connectionFailed;

    // tie udp recv callback to trigger a helix function instead
    m_udpSocket->SetConnectCallback(
        MakeCallback(&HelixSocketImpl::HandleConnectionSucceeded, this),
        MakeCallback(&HelixSocketImpl::HandleConnectionFailed, this)
    );
}

void
HelixSocketImpl::SetCloseCallbacks(
    Callback<void, Ptr<Socket>> normalClose,
    Callback<void, Ptr<Socket>> errorClose)
{
    NS_LOG_FUNCTION(this);

    m_handleNormalClose = normalClose;
    m_handleErrorClose = errorClose;

    // tie udp recv callback to trigger a helix function instead
    m_udpSocket->SetCloseCallbacks(
        MakeCallback(&HelixSocketImpl::HandleNormalClose, this),
        MakeCallback(&HelixSocketImpl::HandleErrorClose, this)
    );
}


void
HelixSocketImpl::SetAcceptCallback(
    Callback<bool, Ptr<Socket>, const Address&> connectionRequest,
    Callback<void, Ptr<Socket>, const Address&> newConnectionCreated)
{
    NS_LOG_FUNCTION(this);

    m_handleConnectionRequest = connectionRequest;
    m_handleNewConnectionCreated = newConnectionCreated;

    // tie udp recv callback to trigger a helix function instead
    m_udpSocket->SetAcceptCallback(
        MakeCallback(&HelixSocketImpl::HandleConnectionRequest, this),
        MakeCallback(&HelixSocketImpl::HandleNewConnectionCreated, this)
    );
}

void
HelixSocketImpl::SetDataSentCallback(Callback<void, Ptr<Socket>, uint32_t> dataSent)
{
    NS_LOG_FUNCTION(this);
    m_handleDataSent = dataSent;

    // tie udp recv callback to trigger a helix function instead
    m_udpSocket->SetDataSentCallback(MakeCallback(&HelixSocketImpl::HandleDataSent, this));
}

void
HelixSocketImpl::SetRecvCallback(Callback<void, Ptr<Socket>> receivedData)
{
    NS_LOG_FUNCTION(this);

    m_handleReceivedData = receivedData;

    // tie udp recv callback to trigger a helix function instead
    m_udpSocket->SetRecvCallback(MakeCallback(&HelixSocketImpl::HandleRecv, this));
}

void
HelixSocketImpl::SetSendCallback(Callback<void, Ptr<Socket>, uint32_t> sendCb)
{
    NS_LOG_FUNCTION(this);
    m_handleSend = sendCb;

    // tie udp send callback to trigger a helix function instead
    m_udpSocket->SetSendCallback(MakeCallback(&HelixSocketImpl::HandleSend, this));
}

void
HelixSocketImpl::HandleConnectionSucceeded(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this);

    m_handleConnectionSucceeded(socket);
}

void
HelixSocketImpl::HandleConnectionFailed(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this);

    m_handleConnectionFailed(socket);
}

void
HelixSocketImpl::HandleNormalClose(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this);

    m_handleNormalClose(socket);
}

void
HelixSocketImpl::HandleErrorClose(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this);

    m_handleErrorClose(socket);
}

bool
HelixSocketImpl::HandleConnectionRequest(Ptr<Socket> socket, const Address& addr)
{
    NS_LOG_FUNCTION(this);

    return m_handleConnectionRequest(socket, addr);
}

void
HelixSocketImpl::HandleNewConnectionCreated(Ptr<Socket> socket, const Address& addr)
{
    NS_LOG_FUNCTION(this);

    m_handleNewConnectionCreated(socket, addr);
}

void
HelixSocketImpl::HandleDataSent(Ptr<Socket> socket, uint32_t size)
{
    NS_LOG_FUNCTION(this);

    m_handleDataSent(this, size);
}

void
HelixSocketImpl::HandleRecv(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this);

    // pass a reference to this socket instead of udp socket
    // this is because these functions will invoke udp functions
    // in a 1-to-1 mapping
    m_handleReceivedData(this);
}

void
HelixSocketImpl::HandleSend(Ptr<Socket> socket, uint32_t num_bytes)
{
    NS_LOG_FUNCTION(this);

    // pass a reference to this socket instead of udp socket
    // this is because these functions will invoke udp functions
    // in a 1-to-1 mapping
    m_handleSend(this, num_bytes);
}

void
HelixSocketImpl::BindToNetDevice(Ptr<NetDevice> netdevice)
{
    NS_LOG_FUNCTION(netdevice);

    // const Address& address = netdevice->GetAddress();
    Address address = Address();
    m_helix_rs_interface->Bind(address);

    m_udpSocket->BindToNetDevice(netdevice);
}

int
HelixSocketImpl::Bind()
{
    NS_LOG_FUNCTION(this);

    // TODO: rust will make a callback to bind
    // TODO: provide address
    Address address = Address();
    m_helix_rs_interface->Bind(address);

    return m_udpSocket->Bind();
}

int
HelixSocketImpl::Bind6()
{
    NS_LOG_FUNCTION(this);

    // TODO: provide address
    Address address = Address();
    m_helix_rs_interface->Bind(address);

    return m_udpSocket->Bind6();
}

int
HelixSocketImpl::Bind(const Address& address)
{
    NS_LOG_FUNCTION(this << address);

    Address addr = Address();
    m_helix_rs_interface->Bind(addr);

    return m_udpSocket->Bind(address);
}

int
HelixSocketImpl::Connect(const Address& address)
{
    NS_LOG_FUNCTION(this << address);

    Address addr = Address();
    m_helix_rs_interface->Connect(addr);

    return m_udpSocket->Connect(address);
}

int
HelixSocketImpl::Listen()
{
    m_helix_rs_interface->Listen();

    return m_udpSocket->Listen();
}

int
HelixSocketImpl::Send(Ptr<Packet> p, uint32_t flags)
{
    NS_LOG_FUNCTION(this << p << flags);

    // TODO: Add logic for maximum packet size (TX available or smth)
    m_helix_rs_interface->Send(p);

    return m_udpSocket->Send(p, flags);
}


int
HelixSocketImpl::SendTo(Ptr<Packet> p, uint32_t flags, const Address& address)
{
    NS_LOG_FUNCTION(this << p << flags << address);


    // TODO: Add logic for adding address
    // TODO: Add logic for maximum packet size (TX available or smth)
    m_helix_rs_interface->Send(p);

    // TODO: Call Helix rs packet encoder implementation
    return m_udpSocket->SendTo(p, flags, address);
}

Ptr<Packet>
HelixSocketImpl::Recv(uint32_t maxSize, uint32_t flags)
{
    NS_LOG_FUNCTION(this << maxSize << flags);

    Ptr<Packet> p = m_udpSocket->Recv(maxSize, flags);
    return m_helix_rs_interface->Recv(p);
}

Ptr<Packet>
HelixSocketImpl::RecvFrom(uint32_t maxSize, uint32_t flags, Address& fromAddress)
{
    NS_LOG_FUNCTION(this << maxSize << flags << fromAddress);
    
    // TODO: Add address
    // TODO: pass received info to m_helix_rs_interface
    Ptr<Packet> p = m_udpSocket->RecvFrom(maxSize, flags, fromAddress);
    return m_helix_rs_interface->Recv(p);
}

int
HelixSocketImpl::Close()
{
    NS_LOG_FUNCTION(this);

    // TODO: rust will make a callback to udp close
    m_helix_rs_interface->Close();

    return m_udpSocket->Close();
}

int
HelixSocketImpl::ShutdownSend()
{
    NS_LOG_FUNCTION(this);
    return m_udpSocket->ShutdownSend();
}

int
HelixSocketImpl::ShutdownRecv()
{
    NS_LOG_FUNCTION(this);
    return m_udpSocket->ShutdownRecv();
}

uint32_t
HelixSocketImpl::GetTxAvailable() const
{
    NS_LOG_FUNCTION(this);
    return m_udpSocket->GetTxAvailable();
}

uint32_t
HelixSocketImpl::GetRxAvailable() const
{
    NS_LOG_FUNCTION(this);
    return m_udpSocket->GetRxAvailable();
}

int
HelixSocketImpl::GetSockName(Address& address) const
{
    NS_LOG_FUNCTION(this << address);
    return m_udpSocket->GetSockName(address);
}

int
HelixSocketImpl::GetPeerName(Address& address) const
{
    NS_LOG_FUNCTION(this << address);
    return m_udpSocket->GetPeerName(address);
}

bool
HelixSocketImpl::SetAllowBroadcast(bool allowBroadcast)
{
    return m_udpSocket->SetAllowBroadcast(allowBroadcast);
}

bool
HelixSocketImpl::GetAllowBroadcast() const
{
    return m_udpSocket->GetAllowBroadcast();
}

void
HelixSocketImpl::Ipv6JoinGroup(Ipv6Address address,
                             Socket::Ipv6MulticastFilterMode filterMode,
                             std::vector<Ipv6Address> sourceAddresses)
{
    NS_LOG_FUNCTION(this << address << &filterMode << &sourceAddresses);
    m_udpSocket->Ipv6JoinGroup(address, filterMode, sourceAddresses);
}



} // namespace ns3
