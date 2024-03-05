#ifndef RDMI_PEER_H
#define RDMI_PEER_H


#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"


// RDMI
#include "rdmi.h"
#include "rdmi-socket.h"
#include "rdmi-header.h"
#include "rdmi-buffer.h"

#include <list>
#include <vector>
#include <map>



namespace ns3
{

class Node;
class Packet;

class RDMI;
class RDMISocket;
class RdmiBuffer;

/**
 * \ingroup rdmi
 * \defgroup rdmipeer RdmiPeer
 */

/**
 * \ingroup rdmipeer
 *
 * \brief An abstraction representing a socket/connection between two peers.
 *
 * TODO: One paragraph description
 */

class RDMIPeer : public Object {
    public:
        /**
         * \brief Get the type ID.
         * \return the object TypeId
         */
        static TypeId GetTypeId();
        RDMIPeer();
        ~RDMIPeer() override;

        /**
         * \brief Set the associated RDMI client
         * \param rdmi the RDMI Client Instance
         */
        void SetRdmi(Ptr<RDMI> rdmi);

    private:
        Ptr<RDMI>   m_rdmi;              // RDMI Client

        /* **************************************************************** */
        /*  PEER SOCKET MANAGEMENT METHODS */
        /* **************************************************************** */
        /*  The Peer socket is in charge of transmitting the datagrams from
            the RdmiSockets to the client. */
        /* **************************************************************** */

    public:

        /**
         * \brief Binds to a client via the RDMISocket
         * \param address address to bind to
         */
        int Bind(const Address& address);

        /**
         * \brief Connects to an address
         * \param address address to bind to
         * 
         * This call might be optional depending on which L4 protocol is 
         * chosen to service RDMI. TCP requires a connect call, but
         * HELIX does not
         */
        int Connect(const Address& address);

        /**
         * \brief Listen for incoming messages to the rdmi_socket
         */
        int Listen();

        /**
         * \brief Close the RDMISocket connection
         * 
         * Closes the real socket if there are no RdmiSockets connected. Sends
         * a close signal to the connected RdmiSocket.
         */
        int Close();

    private:

        /**
         * \brief Total available Rx bytes
         * \param rdmi_socket the RDMISocket
         */
        uint32_t GetTotalRxAvailable() const;
        
        /**
         * \brief Total available Tx byte
         * \param rdmi_socket the RDMISocket
         */
        uint32_t GetTotalTxAvailable() const;

        /**
         * \brief Get the associated peer socket for a given RDMISocket
         */
        Ptr<Socket> GetPeerSocket() const;

    private:
        Ptr<Socket>                     m_peerSocket;       // The socket connected with the peer
        // Combined buffers (the combined data stream of all client the sockets)
        std::list<Ptr<Packet> >         m_requestQueue;     // The incoming buffer
        std::list<Ptr<Packet> >         m_sendQueue;        // The outgoing buffer

        /* **************************************************************** */
        /*  RDMI SOCKET MANAGEMENT METHODS */
        /* **************************************************************** */
        /*  RdmiSockets are socket interfaces used by the client to send data
            to a peer. These are only interfaces, and the "PeerSocket" is
            reponsible for actually transmitting the data. */
        /* **************************************************************** */

    public:
        /**
         * \brief Adds the an RDMISocket to this peer
         * \param rdmi_socket the RDMISocket
         */
        void AddRdmiSocket(Ptr<RDMISocket> rdmi_socket);

        /**
         * \brief Removes an RDMISocket from this peer
         * \param rdmi_socket the RDMISocket
         */
        void RemoveRdmiSocket(Ptr<RDMISocket> rdmi_socket);

        /**
         * \brief Removes an RDMISocket from this peer
         * \param id the RDMISocket id
         */
        void RemoveRdmiSocket(uint16_t id);

    private:
        /**
         * \brief Gets from an id
         * \param id the port assigned to the RDMISocket
         * 
         * This will return a null pointer if none is found.
         */
        Ptr<RDMISocket> GetRdmiSocket(uint16_t id);

        /**
         * \brief How many currently connected RdmiSockets
         */
        size_t GetRdmiSocketCount() const;

        // NOTE: this might end up living in the RDMI Client instance instead
        std::unordered_map<uint16_t, Ptr<RDMISocket> >      m_rdmiSocketList;    // A list of sockets currently interfacing with the client

        /* **************************************************************** */
        /* RDMI SOCKET INTERFACE METHODS */
        /* **************************************************************** */
        /*  The RdmiPeer is charged with distributing and advertising the
            correct amount of resources for each RdmiSocket: i.e.,
            GetTxAvailable should respond with a proportionate amount of
            bytes available that have been allocated in the buffer for a
            given RdmiSocket. */
        /* **************************************************************** */
    
    public:

        /**
         * \brief Sends data down the network
         * \param id the RDMISocket id
         * \param p packet to be sent
         * 
         * In theory, we can carefully choose our advertised Tx such that
         * we can immediately send whatever packet we have been given
         */
        int Send(uint16_t id, Ptr<Packet> p);

        /**
         * \brief Sends data down the network
         * \param id the RDMISocket id
         * \param p packet to be sent
         * \param flags not supported
         * 
         * We do not support flags, so these are discarded.
         */
        int Send(uint16_t id, Ptr<Packet> p, uint32_t flags);

        /**
         * \brief Return a packet to be forwarded to the client via the RDMISocket
         * \param id the RDMISocket id
         * \param maxSize the maximize packet size
         * \param flags flags
         */
        Ptr<Packet> Recv(uint16_t id, uint32_t maxSize, uint32_t flags);

        /**
         * \brief Total available Rx bytes for a given RdmiSocket
         * \param id the RDMISocket id
         */
        uint32_t GetRxAvailable(uint16_t id) const;

        /**
         * \brief Total available Tx bytes for a given RdmiSocket
         * \param id the RDMISocket id
         * 
         * TODO: Account for header bytes when calculating Tx
         * 
         * NOTE: When we promise a RdmiSocket available Tx, we
         * have to guarentee that the RdmiSocket can use all
         * of these bytes. This raises an issue if the client
         * does not use up all of the available Tx, or a
         * maybe a different RdmiSocket is using the Tx at
         * a faster rate and, thus, needs it more than whoever
         * we just promised the Tx to.
         * 
         * For now, we can just divide the available Tx by
         * the number of RDMISocket as a place holder solution.
         * 
         * Solution 1:
         * - Begin with equal division of resources
         * - Proportional allocation of resources depending
         * on MOST RECENT rate consumption (sliding window)
         * 
         * Solution 2:
         * - Begin with equal division of resources
         * - Proportional allocation of resources depending
         * on TOTAL consumption
         * 
         *  Solution 3:
         * - Begin with equal division of resources
         * - Proportional allocation of resources depending
         * on TOTAL rate of consumption
         * 
         * Solution 4: (place holder solution)
         * - Equal division of resources
         */
        uint32_t GetTxAvailable(uint16_t id) const;  


        /* **************************************************************** */
        /* RDMI SOCKET ENCAPSULATION METHODS */
        /* **************************************************************** */
        /* Handles the encapsulation and decapsulation of datagrams to be sent
           between RDMI clients to handle proper multiplexing. */
        /* **************************************************************** */

    private:
        /**
         * \brief Add multiplexing data for RDMI
         * \param p the packet/datagram being sent
         * \param destport the intended destination of this information
         * \param seqno the sequence number of the data
         * \param msg_type the type:'m' for message, 'd' for data, '\O' for none
         * 
         * Adds a tag to a packet with an identifier for the socket
         * connection to be used by the multiplexor.
         */
        void Encapsulate(Ptr<Packet> p, uint16_t dest_port, uint32_t seqno, char msg_type);

        /**
         * \brief Remove multiplexing data from packet
         * \param p the packet/datagram being sent
         * \param h an empty header to be written to
         * 
         * Separates the header and the packet
         */
        uint32_t Decapsulate(Ptr<Packet> p, RDMIHeader h);

    
    /* **************************************************************** */
    /* RDMI SOCKET BUFFER INTERFACE METHODS */
    /* **************************************************************** */
    /*  RDMI Peer is in charge of maintaining buffers for each socket,
        and using said buffers to store outgoing messages. */
    /* **************************************************************** */
    private:
        /**
         * \brief Remove multiplexing data from packet
         * \param id the rdmi socket id
         */
        Ptr<RdmiBuffer> GetMessageBuffer(uint16_t id);

        /**
         * \brief Remove multiplexing data from packet
         * \param id the rdmi socket id
         */
        Ptr<RdmiBuffer> GetDataBuffer(uint16_t id);

        /**
         * \brief Write to socket msg buffer
         * \param id the rdmi socket id
         * \param p the packet/datagram to be stored
         */
        void WriteToSocketMsgBuffer(uint16_t id, Ptr<Packet> p);
        /**
         * \brief Write to socket msg buffer
         * \param id the rdmi socket id
         * \param p the packet/datagram to be stored
         */
        void WriteToSocketDataBuffer(uint16_t id, Ptr<Packet> p);

    private:
        std::unordered_map<uint16_t, Ptr<RdmiBuffer> >      m_rdmiSocketMsgBuffers;    // A list of message buffers associated with each socket
        std::unordered_map<uint16_t, Ptr<RdmiBuffer> >      m_rdmiSocketDataBuffers;    // A list of data buffers associated with each socket

};


} // namespace ns3

#endif /* RDMI_PEER_H */