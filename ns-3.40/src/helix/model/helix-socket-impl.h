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
 * \ingroup helix
 *
 * \brief A sockets interface to HELIX
 *
 * This class subclasses HelixSocket, and provides a socket interface
 * to an ns3 implementation of HELIX. This implementation is built
 * on UDP.
 *
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

    /* -------------------- Setting Callbacks -------------------- */

    /**
     * \brief Specify callbacks to allow the caller to determine if
     * the connection succeeds of fails.
     * \param connectionSucceeded this callback is invoked when the
     *        connection request initiated by the user is successfully
     *        completed. The callback is passed  back a pointer to
     *        the same socket object.
     * \param connectionFailed this callback is invoked when the
     *        connection request initiated by the user is unsuccessfuly
     *        completed. The callback is passed back a pointer to the
     *        same socket object.
     */
    void SetConnectCallback(Callback<void, Ptr<Socket>> connectionSucceeded,
                            Callback<void, Ptr<Socket>> connectionFailed) override;
    /**
     * \brief Detect socket recv() events such as graceful shutdown or error.
     *
     * For connection-oriented sockets, the first callback is used to signal
     * that the remote side has gracefully shut down the connection, and the
     * second callback denotes an error corresponding to cases in which
     * a traditional recv() socket call might return -1 (error), such
     * as a connection reset.  For datagram sockets, these callbacks may
     * never be invoked.
     *
     * \param normalClose this callback is invoked when the
     *        peer closes the connection gracefully
     * \param errorClose this callback is invoked when the
     *        connection closes abnormally
     */
    void SetCloseCallbacks(Callback<void, Ptr<Socket>> normalClose,
                           Callback<void, Ptr<Socket>> errorClose) override;
    /**
     * \brief Accept connection requests from remote hosts
     * \param connectionRequest Callback for connection request from peer.
     *        This user callback is passed a pointer to this socket, the
     *        ip address and the port number of the connection originator.
     *        This callback must return true to accept the incoming connection,
     *        false otherwise. If the connection is accepted, the
     *        "newConnectionCreated" callback will be invoked later to
     *        give access to the user to the socket created to match
     *        this new connection. If the user does not explicitly
     *        specify this callback, all incoming  connections will be refused.
     * \param newConnectionCreated Callback for new connection: when a new
     *        is accepted, it is created and the corresponding socket is passed
     *        back to the user through this callback. This user callback is
     *        passed a pointer to the new socket, and the ip address and
     *        port number of the connection originator.
     */
    void SetAcceptCallback(Callback<bool, Ptr<Socket>, const Address&> connectionRequest,
                           Callback<void, Ptr<Socket>, const Address&> newConnectionCreated) override;
    /**
     * \brief Notify application when a packet has been sent from transport
     *        protocol (non-standard socket call)
     * \param dataSent Callback for the event that data is sent from the
     *        underlying transport protocol.  This callback is passed a
     *        pointer to the socket, and the number of bytes sent.
     */
    void SetDataSentCallback(Callback<void, Ptr<Socket>, uint32_t> dataSent) override;

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
    void SetRecvCallback(Callback<void, Ptr<Socket>> receivedData) override;

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
    void SetSendCallback(Callback<void, Ptr<Socket>, uint32_t> sendCb) override;

    /* -------------------- Callbacks Passed to UDP -------------------- */

    void HandleConnectionSucceeded(Ptr<Socket> socket);
    void HandleConnectionFailed(Ptr<Socket> socket);
    void HandleNormalClose(Ptr<Socket> socket);
    void HandleErrorClose(Ptr<Socket> socket);
    bool HandleConnectionRequest(Ptr<Socket> socket, const Address& addr);
    void HandleNewConnectionCreated(Ptr<Socket> socket, const Address& addr);
    void HandleDataSent(Ptr<Socket> socket, uint32_t size);
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
    // /**
    //  * \brief UdpSocketFactory friend class.
    //  * \relates UdpSocketFactory
    //  * 
    //  * Note: I do not really know what this is doing here.
    //  */
    // friend class UdpSocketFactory;


    // Connections to other layers of UDP
    Ptr<Node> m_node;          //!< the associated node
    Ptr<Socket> m_udpSocket;  //!< the associated socket
    Ptr<HelixL4Protocol> m_helix;
    Ptr<HelixRsInterface> m_helix_rs_interface;

    Callback<void, Ptr<Socket>> m_handleConnectionSucceeded;
    Callback<void, Ptr<Socket>> m_handleConnectionFailed;
    Callback<void, Ptr<Socket>> m_handleNormalClose;
    Callback<void, Ptr<Socket>> m_handleErrorClose;
    Callback<bool, Ptr<Socket>, const Address&> m_handleConnectionRequest;
    Callback<void, Ptr<Socket>, const Address&> m_handleNewConnectionCreated;
    Callback<void, Ptr<Socket>, uint32_t> m_handleDataSent;
    Callback<void, Ptr<Socket>> m_handleReceivedData;
    Callback<void, Ptr<Socket>, uint32_t> m_handleSend;
    
};

} // namespace ns3

#endif /* HELIX_SOCKET_IMPL_H */
