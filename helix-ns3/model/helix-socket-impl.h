/*
 * An normal socket interface for Helix
 *
 * Socket Interface
 * Interface:
 * - BindToNetDevice()
 * - Connect()
 * - Bind()
 * - Bind6()
 * - Listen()
 * - ShutdownSend()
 * - Recv()
 * - RecvFrom()
 * - Send()
 * - SendTo()
 * - Close()
 * 
 * Design Principles:
 * - Minimal public interface necessary
 */
#ifndef HELIX_SOCKET_IMPL_H
#define HELIX_SOCKET_IMPL_H


#include "helix-socket.h"
#include "helix-l4-protocol.h"
#include "helix-rs-interface.h"


#include "ns3/internet-module.h"

#include "ns3/icmpv4.h"
#include "ns3/ipv4-interface.h"

#include "ns3/callback.h"
#include "ns3/ipv4-address.h"
#include "ns3/ptr.h"
#include "ns3/socket.h"
#include "ns3/traced-callback.h"

#include <queue>
#include <stdint.h>

namespace ns3
{

class Ipv4EndPoint;
class Ipv6EndPoint;

class Node;
class Packet;

class HelixL4Protocol;
class HelixRsInterface;

class Ipv6Header;
class Ipv6Interface;

/**
 * \ingroup socket
 * \ingroup udp
 *
 * \brief A sockets interface to UDP
 *
 * This class subclasses ns3::UdpSocket, and provides a socket interface
 * to ns3's implementation of UDP.
 *
 * For IPv4 packets, the TOS is set according to the following rules:
 * - if the socket is connected, the TOS set for the socket is used
 * - if the socket is not connected, the TOS specified in the destination address
 *   passed to SendTo is used, while the TOS set for the socket is ignored
 * In both cases, a SocketIpTos tag is only added to the packet if the resulting
 * TOS is non-null. The Bind and Connect operations set the TOS for the
 * socket to the value specified in the provided address.
 * If the TOS determined for a packet (as described above) is not null, the
 * packet is assigned a priority based on that TOS value (according to the
 * Socket::IpTos2Priority function). Otherwise, the priority set for the
 * socket is assigned to the packet. Setting a TOS for a socket also sets a
 * priority for the socket (according to the Socket::IpTos2Priority function).
 * A SocketPriority tag is only added to the packet if the resulting priority
 * is non-null.
 */

class HelixSocketImpl : public HelixSocket
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    /**
     * Create an unbound Helix socket.
     */
    HelixSocketImpl();
    ~HelixSocketImpl() override;

    /**
     * \brief Set the associated node.
     * \param node the node
     */
    void SetNode(Ptr<Node> node);
    
    /**
     * \brief Set the associated HELIX L4 protocol.
     * \param helix the HELIX L4 protocol
     */
    void SetHelix(Ptr<HelixL4Protocol> helix);

    /**
     * \brief Set the associated UDP socket.
     * \param udp_socket the UDP socket
     */
    void SetUdpSocket(Ptr<Socket> udp_socket);

    SocketErrno GetErrno() const override;
    SocketType GetSocketType() const override;
    Ptr<Node> GetNode() const override;

    /**
     * \brief Notify application when new data is available to be read.
     *
     *        This callback is intended to notify a socket that would
     *        have been blocked in a blocking socket model that data
     *        is available to be read.
     * \param receivedData Callback for the event that data is received
     *        from the underlying transport protocol.  This callback
     *        is passed a pointer to the socket.
     * 
     * Must override this to give HELIX access packets when UDP receives
     * data via a callback function. We pass a HELIX method called
     * HandleRecv() to UDP as the callback. HandleRecv() will then
     * invoke the original receivedData callback.
     * 
     * Because we call UDP functions in a 1-to-1 mapping with
     * HelixSocketImpl functions (recv(), send(), bind(), etc.
     * where each function invokes the UDP counter part), it
     * will be as if the same callback was passed directly
     * to UDP, the only difference is that HelixSocketImpl class
     * will now have access to the data passing through via these
     * functions (recv(), send(), bind(), etc.), so we can tap
     * into the stream, modify the packets, and pass it along.
     * 
     * \attention To implement you must mark Socket::SetRecvCallback() as virtual
     */
    // void SetRecvCallback(Callback<void, Ptr<Socket>> receivedData) override;
    void SetRecvCallback(Callback<void, Ptr<Socket>> receivedData);

    /**
     * \brief Notify application when space in transmit buffer is added
     *
     *        This callback is intended to notify a
     *        socket that would have been blocked in a blocking socket model
     *        that space is available in the transmit buffer and that it
     *        can call Send() again.
     *
     * \param sendCb Callback for the event that the socket transmit buffer
     *        fill level has decreased.  This callback is passed a pointer to
     *        the socket, and the number of bytes available for writing
     *        into the buffer (an absolute value).  If there is no transmit
     *        buffer limit, a maximum-sized integer is always returned.
     * 
     * This functions similarly to SetRecvCallback.
     */
    // void SetSendCallback(Callback<void, Ptr<Socket>, uint32_t> sendCb) override;
    void SetSendCallback(Callback<void, Ptr<Socket>, uint32_t> sendCb);

     /**
     * \brief Callback invoked by UDP when it receives data
     * \param socket the udp socket
     * 
     */
    void HandleRecv(Ptr<Socket> socket);


     /**
     * \brief Callback invoked by UDP when it sends data
     * \param socket the udp socket
     * \param num_bytes the number of bytes available for writing
     *        into the buffer (an absolute value).  If there is no transmit
     *        buffer limit, a maximum-sized integer is always returned.
     */
    void HandleSend(Ptr<Socket> socket, uint32_t num_bytes);

    /* -------------------- HELIX Interface -------------------- */
    void BindToNetDevice(Ptr<NetDevice> netdevice) override;
    int Connect(const Address& address) override;
    int Bind() override;
    int Bind6() override;
    int Bind(const Address& address) override;
    int Listen() override;
    int Send(Ptr<Packet> p, uint32_t flags) override;
    int SendTo(Ptr<Packet> p, uint32_t flags, const Address& address) override;
    Ptr<Packet> Recv(uint32_t maxSize, uint32_t flags) override;
    Ptr<Packet> RecvFrom(uint32_t maxSize, uint32_t flags, Address& fromAddress) override;
    int Close() override;


  
    int ShutdownSend() override;
    int ShutdownRecv() override;
    uint32_t GetTxAvailable() const override;
    uint32_t GetRxAvailable() const override;
    int GetSockName(Address& address) const override;
    int GetPeerName(Address& address) const override;
    bool SetAllowBroadcast(bool allowBroadcast) override;
    bool GetAllowBroadcast() const override;
    void Ipv6JoinGroup(Ipv6Address address,
                       Socket::Ipv6MulticastFilterMode filterMode,
                       std::vector<Ipv6Address> sourceAddresses) override;


  private:
    /**
     * \brief UdpSocketFactory friend class.
     * \relates UdpSocketFactory
     */
    friend class UdpSocketFactory;
    // invoked by Udp class


    // Connections to other layers of TCP/IP
    Ptr<Node> m_node;                 //!< the associated node
    Ptr<Socket> m_udp_socket;  //!< the associated socket
    Ptr<HelixL4Protocol> m_helix;
    Ptr<HelixRsInterface> m_helix_rs_interface;

    Callback<void, Ptr<Socket>> m_handle_recv;
    Callback<void, Ptr<Socket>, uint32_t> m_handle_send;

    
};

} // namespace ns3

#endif /* HELIX_SOCKET_IMPL_H */
