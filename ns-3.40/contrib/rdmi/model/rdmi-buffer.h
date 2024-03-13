#ifndef RDMI_BUFFER_H
#define RDMI_BUFFER_H


#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"

#include "rdmi-node.h"

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup rdmibuffer TODO: Description of the rdmi buffer
 */

namespace ns3 {
namespace rdmi {

// class Node;
// class Packet;

class RdmiNode;

// maximum number of nodes
static const uint16_t RDMI_BUFFER_MAX_NODES = 65535; // more or less arbitrary

/**
 * \ingroup rdmibuffer
 *
 * \brief A linked list buffer for rdmi
 *
 * This is an rdmi buffer. This is a virtual buffer made of a linked list of
 * nodes, each containing a real buffer. RdmiBuffers are not responsible for
 * allocating space for their nodes. Nodes are responsible for this.
 * 
 * RdmiBuffers go about getting nodes in two ways: (1) they can either allocate
 * them or (2) the can be donated nodes from other buffers.
 * 
 * RdmiBuffers can swap full nodes (nodes that have already been written to) with
 * empty ones via a swap mechanism.
 * 
 * NOTE: the donate and swap mechanisms have relatively few safe guards in place,
 * use them carefully
 */

class RdmiBuffer : public Object {
    public:
        /**
         * \brief Get the type ID.
         * \return the object TypeId
         */
        static TypeId GetTypeId();

        /**
         * \brief Construct an Rdmi Buffer
         */
        RdmiBuffer();
        ~RdmiBuffer() override;

        /* **************************************************************** */
        /* RDMI BUFFER DATA MANIPULATION METHODS */
        /* **************************************************************** */
        /* Methods for reading, writing, etc. data to/from the buffer. */
        /* **************************************************************** */

    public:

        /**
         * \brief Writes a datagram to the buffer
         * \param buffer the input buffer to read from
         * \param size the number of bytes to read from the buffer
         */
        void WriteBuffer(uint8_t* buffer, uint32_t size);

        /**
         * \brief Writes a packet to the buffer
         * \param p the packet to write to the buffer
         */
        void WritePacket(Ptr<Packet> p);

        /**
         * \brief Writes a datagram to the buffer
         * \param buffer the input buffer to read from
         * \param size the number of bytes to read from the buffer
         */
        void Write(uint8_t* buffer, uint32_t size);

        /**
         * \brief Reads exactly one datagram from the buffer
         * \param buffer the input buffer to write to
         * 
         * Reading from a buffer can only be performed once as the data will be marked
         * as copied, and the buffer will no longer be responsible for maintaining it.
         * 
         * It is assumed that the provided buffer has the required space to perform
         * the read action (GetAvailableDatagramSize() is the number of bytes needed).
         */
        void ReadDatagram(uint8_t* buffer);

        /**
         * \brief Returns the number of bytes used
         */
        uint16_t GetSizeBytes() const;

        /**
         * \brief Returns the total number of bytes in the rdmi buffer (node count * node buff size)
         */
        uint16_t GetCapacityBytes() const;

        /**
         * \brief Updates the total number of bytes in the rdmi buffer
         * \param capacity
         */
        void SetCapacityBytes(uint32_t capacity);

        /**
         * \brief returns the size of the first available datagram ready to be transmitted
         */
        uint16_t GetAvailableDatagramSize() const;

        /**
         * \brief Clear buffer
         * 
         * This clears every single buffer. This takes
         * O(m_node_cap) time.
         * 
         * TODO: this might not be necessary with proper
         * memory management.
         */
        void Clear();

        
        /* **************************************************************** */
        /* RDMI NODE MANIPULATION METHODS */
        /* **************************************************************** */
        /* Methods for manipulating nodes stored in buffer.
         * TODO: Make two children classes, one for transport buffer and
         * one for socket buffer and use protected/friend classes to make
         * the swap, donate, etc. methods only accessible to each other. */
        /* **************************************************************** */

    public:
        /**
         * \brief allocates a given number of nodes for the buffer
         * 
         * This allocates a circularly linked list of nodes. It will
         * not go above the max number of nodes allowed.
        */
       void AllocateNodes();

        /**
         * \brief allocates a given number of nodes for the buffer
         * \param size the number of nodes to allocate
         * 
         * This allocates a circularly linked list of nodes. It will
         * not go above the max number of nodes allowed.
        */
       void AllocateNodes(uint16_t size);

        /**
         * \brief deallocates all nodes in the buffer
        */
       void DeallocateNodes();

        /**
         * \brief Given a list of nodes, will insert the list and change the pointer
         * value to point to the list of nodes
         * \param target the buffer that is to receive the empty nodes
         * \param size the number of nodes to swap
         * 
         * This function will fully handle swapping of information including the
         * relinking of each linked list. The pointer to the starting empty node
         * is returned. It is assumed that the full nodes are all valid.
         * 
         * This operation takes O(size) time. The target refers to the buffer that
         * is trading nodes with this buffer. The recipient refers to this buffer.
         * The target buffer should always be a socket buffer, and the recipient
         * buffer should always be the transmission buffer. This way, we know that
         * we are always distributing empty nodes and we are receiving full nodes.
         * 
         * This function assumes that m_node_cap accurately represents the number
         * of nodes that are in this buffer (both full and empty).
         * 
         * TODO: have some sort of locking mechanism while this is being performed
         * TODO: should add restriction on only moving an entire datagram?
         * TODO: edgecase when (target start) == (target end -> next) (aka n-1 buffer)
         * TODO: edgecase when (target start) == (target end) (aka full buffer)
         */
        void SwapNodes(Ptr<RdmiBuffer> target, uint16_t size);

        /**
         * \brief removes nodes from the circular buffer and adds to the target
         * \param target the target to receive the nodes
         * \param size the number of nodes to fetch
         * 
         * This operation takes O(size) time. This handles all linking.
         * 
         * TODO: When size = m_node_cap, this should only take O(1) time
         * 
         * TODO: have some sort of locking mechanism while this is being performed
         * TODO: edgecase when (target start) == (target end -> next) (aka n-1 buffer)
         * TODO: edgecase when (target start) == (target end) (aka full buffer)
         */
        void DonateNodes(Ptr<RdmiBuffer> target, uint16_t size);

        /**
         * \brief Get the start node
         * \returns a pointer to the start node
         */
        RdmiNode* GetStartNode() const;

        /**
         * \brief Get the end node
         * \returns a pointer to the end node
         */
        RdmiNode* GetEndNode() const;

        /**
         * \brief Set the start node
         */
        void SetStartNode(RdmiNode* start);

        /**
         * \brief Set the end node
         */
        void SetEndNode(RdmiNode* end);

        /**
         * \brief Returns the number of nodes used
         * \return returns the number of nodes used
         */
        uint16_t GetNodeCount() const;

        /**
         * \brief Returns the total number of nodes in the buffer that have been allocated
         * \return returns the total number of nodes in the rdmi buffer
         */
        uint16_t GetNodeCapacity() const;

        /**
         * \brief Get the number of nodes available to be written to
         * \return returns the number of nodes available
         */
        uint16_t GetNodeAvailable() const;

        /**
         * \brief Sets the node count
         * \param count the new node count
         */
        void SetNodeCount(uint16_t count);

        /**
         * \brief Sets the node capacity
         * \param capacity the new node capacity
         * 
         * TODO: implement an upper bound assertion for capacity
         */
        void SetNodeCapacity(uint16_t capacity);


    private:
        RdmiNode*       m_start;            // pointer to item before the first written node
        RdmiNode*       m_end;              // pointer to last item in linked list that has been written to

        uint16_t        m_node_count;       // number of used nodes in the linked list (ie those that cannot be written to)
        uint16_t        m_node_cap;         // total number of nodes in the linked list 

        // TODO: remove these two, these are not really necessary, can be calculated by nodes
        uint32_t        m_byte_count;       // number of bytes used
        uint32_t        m_byte_cap;         // total number of bytes that can be stored

        // TODO: write a function to return this value
        uint32_t        m_available_datagram_size;  // the size of the first available datagram ready to be transmitted

        // meta data
        // TODO: combine dest_port and message type
        uint16_t        m_dest_port;        // destination that this data is intended for
        uint32_t        m_seqno;            // sequence number of the data for the transmission    
        char            m_msg_type;         // message type 'm' for message, 'd' for data, \O for none

};

} // namespace rdmi
} // namespace ns3

#endif /* RDMI_BUFFER_H */
