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



namespace ns3 {
namespace rdmi {

// class Node;
// class Packet;

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

// TODO: find a good number
const uint32_t MAX_TX_BUFF_SIZE = 2048; // in nodes 
const uint16_t MSG_BUFF_SIZE = 16; // in nodes
const uint16_t DATA_BUFF_SIZE = 48; // in nodes

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

    private:

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
         * \brief Sends a packet to the msg buff
         * \param id the RDMISocket id
         * \param p packet to be sent
         * 
         */
        void SocketSendToMsgBuff(uint16_t id, Ptr<Packet> p);

        /**
         * \brief Sends a packet to the data buff
         * \param id the RDMISocket id
         * \param p packet to be sent
         * 
         */
        void SocketSendToDataBuff(uint16_t id, Ptr<Packet> p);

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
         * 
         * Solution: prebuffering
         */

        /**
         * \brief Get Tx Available for message buffer
         * \param id the RDMISocket id
        */
        uint32_t GetTxAvailableMsgBuff(uint16_t id) const;

        /**
         * \brief Get Tx Available for data buffer
         * \param id the RDMISocket id
        */
        uint32_t GetTxAvailableDataBuff(uint16_t id) const;  


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
         * 
         * If first bit is 1, it is a Msg Buffer
         * The other bits are the socket address
         * 
         * // TODO: make sure this first bit is never used
         */
        uint16_t GetMsgBufferId(uint16_t id) const;

        /**
         * \brief Remove multiplexing data from packet
         * \param id the rdmi socket id
         * 
         * If first bit is 0, it is a Msg Buffer
         * The other bits are the socket address
         * 
         * // TODO: make sure this first bit is never used
         */
        uint16_t GetDataBufferId(uint16_t id) const;

        /**
         * \brief Remove multiplexing data from packet
         * \param id the rdmi socket id
         */
        Ptr<RdmiBuffer> GetMsgBuffer(uint16_t id) const;

        /**
         * \brief Remove multiplexing data from packet
         * \param id the rdmi socket id
         */
        Ptr<RdmiBuffer> GetDataBuffer(uint16_t id) const;

        /**
         * \brief Can create a new buffer pair given number of available bytes
         * \returns true if there is enough bytes for adding a buffer
         * 
         * A buffer pair just refers to a data buffer and a message buffer.
         * These appear in pairs because each socket will have one of each:
         * one for sending messages (in-order) and one for sending data
         * (un-ordered). Generally, the databuffer is assumed to be larger
         * as more data will be sent.
         */
        bool CanCreateBufferPair();

        /**
         * \brief Create a data buffer and message buffer
         * \param id the RDMISocket id
         * 
         * A buffer pair just refers to a data buffer and a message buffer.
         * These appear in pairs because each socket will have one of each:
         * one for sending messages (in-order) and one for sending data
         * (un-ordered). Generally, the databuffer is assumed to be larger
         * as more data will be sent.
         * 
         * There must be enough free nodes in the transmission buffer to
         * perform this action.
         */
        void CreateBufferPair(uint16_t id);

        /**
         * \brief Create a data buffer and message buffer
         * \param id the RDMISocket id
         * 
         * These nodes are cleared and reallocated to the buffer
         */
        void RemoveBufferPair(uint16_t id);

        /**
         * \brief Write to socket msg buffer
         * \param id the rdmi socket id
         * \param p the packet/datagram to be stored
         * 
         * There must be space in the buffer for the packet.
         * 
         * TODO: is this function really necessary?
         */
        void WriteToMsgBuffer(uint16_t id, Ptr<Packet> p);

        /**
         * \brief Write to socket msg buffer
         * \param id the rdmi socket id
         * \param p the packet/datagram to be stored
         * 
         * There must be space in the buffer for the packet.
         * 
         * TODO: is this function really necessary?
         */
        void WriteToDataBuffer(uint16_t id, Ptr<Packet> p);

    private:
        Ptr<RdmiBuffer> m_tx_buffer; // transmission buffer
        std::unordered_map<uint16_t, Ptr<RdmiBuffer> >      m_socketBuffers;    // A list of message buffers associated with each socket
};

} // namespace rdmi
} // namespace ns3

#endif /* RDMI_PEER_H */