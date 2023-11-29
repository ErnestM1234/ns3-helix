/**
 * 
 * An instance of this is kept in HelixSocketFactory
 * 
 * Interface
 * - CreateSocket()
 * 
 * Design Principles:
 * - Minimal public interface necessary
 * 
 * Notes:
 * I think this is a singleton
 * 
*/


#ifndef HELIX_L4_PROTOCOL_H
#define HELIX_L4_PROTOCOL_H


#include "helix-socket-impl.h"

#include "ns3/internet-module.h"

#include "ns3/ip-l4-protocol.h"
#include "ns3/udp-l4-protocol.h"

#include "ns3/packet.h"
#include "ns3/ptr.h"

#include <stdint.h>
#include <unordered_map>

namespace ns3
{

class Node;
class Socket;
class Ipv4EndPointDemux;
class Ipv4EndPoint;
class Ipv6EndPointDemux;
class Ipv6EndPoint;
class HelixSocketImpl;
class NetDevice;

/**
 * \ingroup internet
 * \defgroup helix HELIX
 *
 */

/**
 * \ingroup helix
 * \brief Implementation of the a simple socket protocol
 */


class HelixL4Protocol : public IpL4Protocol
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    static const uint8_t PROT_NUMBER; //!< protocol number (0x11)

    HelixL4Protocol();
    ~HelixL4Protocol() override;

    // Delete copy constructor and assignment operator to avoid misuse
    HelixL4Protocol(const HelixL4Protocol&) = delete;
    HelixL4Protocol& operator=(const HelixL4Protocol&) = delete;

    /**
     * Set node associated with this stack
     * \param node the node
     */
    void SetNode(Ptr<Node> node);
    int GetProtocolNumber() const override;

    /**
     * 
     * \return A smart Socket pointer to a HelixSocket, allocated by this instance
     * of the HELIX protocol
     */
    Ptr<Socket> CreateSocket();

    // inherited from Ipv4L4Protocol
    // TODO: fill out these functions
    IpL4Protocol::RxStatus Receive(Ptr<Packet> p,
                                   const Ipv4Header& header,
                                   Ptr<Ipv4Interface> interface) override;
    IpL4Protocol::RxStatus Receive(Ptr<Packet> p,
                                   const Ipv6Header& header,
                                   Ptr<Ipv6Interface> interface) override;

    // TODO: fill out these functions
    void ReceiveIcmp(Ipv4Address icmpSource,
                     uint8_t icmpTtl,
                     uint8_t icmpType,
                     uint8_t icmpCode,
                     uint32_t icmpInfo,
                     Ipv4Address payloadSource,
                     Ipv4Address payloadDestination,
                     const uint8_t payload[8]) override;
    void ReceiveIcmp(Ipv6Address icmpSource,
                     uint8_t icmpTtl,
                     uint8_t icmpType,
                     uint8_t icmpCode,
                     uint32_t icmpInfo,
                     Ipv6Address payloadSource,
                     Ipv6Address payloadDestination,
                     const uint8_t payload[8]) override;

    // From IpL4Protocol
    void SetDownTarget(IpL4Protocol::DownTargetCallback cb) override;
    void SetDownTarget6(IpL4Protocol::DownTargetCallback6 cb) override;
    // From IpL4Protocol
    IpL4Protocol::DownTargetCallback GetDownTarget() const override;
    IpL4Protocol::DownTargetCallback6 GetDownTarget6() const override;
    // From UdpL4Protocol
    // void SetDownTarget(UdpL4Protocol::DownTargetCallback cb) override;

    void SetUdp(UdpL4Protocol udp);

  
  protected:
    void DoDispose() override;
    /*
     * This function will notify other components connected to the node that a new stack member is
     * now connected This will be used to notify Layer 3 protocol of layer 4 protocol stack to
     * connect them together.
     */
    void NotifyNewAggregate() override;

  private:
    Ptr<Node> m_node;                //!< The node this stack is associated with

    std::unordered_map<uint64_t, Ptr<HelixSocketImpl>>
        m_sockets;             //!< Unordered map of socket IDs and corresponding sockets
    uint64_t m_socketIndex{0}; //!< Index of the next socket to be created
    IpL4Protocol::DownTargetCallback m_downTarget;   //!< Callback to send packets over IPv4
    IpL4Protocol::DownTargetCallback6 m_downTarget6; //!< Callback to send packets over IPv6
};

} // namespace ns3

#endif /* HELIX_L4_PROTOCOL_H */
