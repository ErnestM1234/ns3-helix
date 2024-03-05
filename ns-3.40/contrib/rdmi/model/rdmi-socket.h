#ifndef RDMI_SOCKET_H
#define RDMI_SOCKET_H



#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/socket.h"

#include <stdint.h>

#include "rdmi.h"
#include "rdmi-peer.h"

namespace ns3
{

class Node;
class Packet;

class RDMI;
class RDMIPeer;


/**
 * \ingroup rdmi
 * \defgroup rdmisocket RdmiSocket
 */

/**
 * \ingroup rdmisocket
 *
 * \brief A class with a socket interface for using RDMI.
 *
 * This is not the true socket that will be sending the data. This is just a class that extends a socket interface for easy client
 * side implementation. All data will be passed to the associated RDMI_Peer object which will then pass the information via socket
 * (datagram or stream) to the peer.
 */

class RDMISocket : public Socket {
    public:
        /**
         * \brief Get the type ID.
         * \return the object TypeId
         */
        static TypeId GetTypeId();
        RDMISocket();
        ~RDMISocket() override;


        /**
         * \brief Set the associated RDMI client
         * \param rdmi the RDMI Client Instance
         */
        void SetRdmi(Ptr<RDMI> rdmi);
        /**
         * \brief Set the associated RDMI peer
         * \param peer the RDMI Peer Instance
         */
        void SetPeer(Ptr<RDMIPeer> peer);

        /**
         * \brief Get the destination port
         */
        uint16_t GetDestinationPort() const;

        /**
         * \brief Get the port
         */
        uint16_t GetPort() const;


        SocketErrno GetErrno() const override; // TODO: Revisit impl
        SocketType GetSocketType() const override; // TODO: Revisit impl
        Ptr<Node> GetNode() const override;
        int Bind() override; // TODO: Figure out what bind operations want to provide
        int Bind6() override;
        int Bind(const Address& address) override;
        int Close() override;
        int ShutdownSend() override;
        int ShutdownRecv() override;
        int Connect(const Address& address) override;
        int Listen() override;
        uint32_t GetTxAvailable() const override;
        int Send(Ptr<Packet> p, uint32_t flags) override;
        int SendTo(Ptr<Packet> p, uint32_t flags, const Address& address) override;
        uint32_t GetRxAvailable() const override;
        Ptr<Packet> Recv(uint32_t maxSize, uint32_t flags) override;
        Ptr<Packet> RecvFrom(uint32_t maxSize, uint32_t flags, Address& fromAddress) override;
        int GetSockName(Address& address) const override;
        int GetPeerName(Address& address) const override;
        void BindToNetDevice(Ptr<NetDevice> netdevice) override;
        bool SetAllowBroadcast(bool allowBroadcast) override;
        bool GetAllowBroadcast() const override;
        void Ipv6JoinGroup(Ipv6Address address,
                        Socket::Ipv6MulticastFilterMode filterMode,
                        std::vector<Ipv6Address> sourceAddresses) override;
        
    private:
        uint16_t                        m_port;             // id number for socket
        uint16_t                        m_dest_port;        // destination port
        Socket::SocketErrno             m_errno;            //!< Socket error code
        Ptr<Node>                       m_node;             //!< the associated node

        std::vector<Ptr<RDMISocket> >   m_rdmiSocketList;   // A list of sockets currently interfacing with the client
        Ptr<RDMI>                       m_rdmi_client;       //!< the associated rdmi instance
        Ptr<RDMIPeer>                   m_rdmi_peer;         //!< the associated rdmi peer
};


} // namespace ns3

#endif /* RDMI_SOCKET_H */