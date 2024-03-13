#ifndef RDMI_NODE_H
#define RDMI_NODE_H


#include "ns3/buffer.h"
#include "ns3/packet.h"

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup rdminode TODO: Description of the rdmi buffer
 */

namespace ns3 {
namespace rdmi {

// class ns3::Buffer;
// class ns3::Packet;

static const uint32_t RDMI_BUFF_MAX_SIZE_BYTES = 2048; // more or less arbitrary
static const uint32_t RDMI_BUFF_DEFAULT_SIZE_BYTES = 2048; // more or less arbitrary

/**
 * \ingroup rdminode
 *
 * \brief A node for a rdmi buffer
 * 
 * This class represents a single node in the rdmi virtual buffer for
 * a single peer. This is a linked list of buffers. This holds a
 * buffer of data to transmit.
 * 
 * TODO: Explore this idea:
 * It contains a pointer to a real buffer of a fixed size.
 * This is preallocated, so it only maintains references to
 * that buffer.
 * 
 * This class does not extend ns3:object to avoid space overhead.
 * 
 */
class RdmiNode {
    public:


        /**
         * \brief Construct an RdmiNode instance
         * 
         * Will allocate maximum allowed number of bytes
        */
        RdmiNode();

        /**
         * \brief Construct an RdmiNode instance
         * \param capacity the total number of nodes to allocate
        */
        RdmiNode(uint32_t capacity); // this size should be standardized
        ~RdmiNode();

        /**
         * \brief Get the next node in the linked list
         * \returns The next node in the linked list
        */
        RdmiNode* GetNext();

        /**
         * \brief Set the next node in the linked list
         * \param next The next node in the linked list
        */
        void SetNext(RdmiNode* next);

        /**
         * \brief returns the size of the datagram
         * 
         * This allows us to track where a datagram starts. This is
         * especially useful when a datagram exceeds the length of
         * a single node. This will represent the total length in
         * bytes of a datagram.
         * 
         * This is only given by the node in which a datagram starts.
        */
        uint32_t GetDatagramLength();

        /**
         * \brief Record the length of the datagram being stored
         * \param len
         * 
         * This allows us to track where a datagram starts. This is
         * especially useful when a datagram exceeds the length of
         * a single node. This will represent the total length in
         * bytes of a datagram.
        */
        void SetDatagramLength(uint32_t len);

        /**
         * \brief returns if the node in question contains a header
         * 
         * This allows us to track where a datagram starts. This is
         * especially useful when a datagram exceeds the length of
         * a single node. This will represent the total length in
         * bytes of a datagram.
         * 
         * This is only given by the node in which a datagram starts.
        */
        bool ContainsDatagramStart();

        /**
         * \brief Marks the buffer node as empty and ready to receive memory
         *
         * This occurs after a call to send
         */
        void Reset();


        /**
         * \brief Get the current amount of used bytes in the node's buffer
         * 
         * \return Returns the current amount of used bytes in the node's buffer
         * 
         */
        uint32_t GetSize();
        
        /**
         * \brief Get the capacity of the nodes buffer
         * 
         * \return Returns the total capacity the node's buffer
         * 
         */
        uint32_t GetCapacity() const;

        /**
         * \brief Writes a packet to the buffer
         * \param p the packet to write to the buffer
         * \returns number of bytes written
         */
        uint32_t WritePacket(Ptr<Packet> p);

        /**
         * \brief Write to the buffer
         * \param buffer a byte buffer to copy in the internal buffer.
         * \param size number of bytes to copy.
         * 
         * \returns the number of bytes written
         *
         * Write the data in buffer and advance the iterator position
         * by size bytes.
         */
        uint32_t Write(const uint8_t* buffer, uint32_t size);

        /**
         * \brief Read from the buffer
         * \param buffer buffer to copy data into
         * \param size number of bytes to copy
         * 
         * \returns the number of bytes that could be read
         *
         * Copy size bytes of data from the internal buffer to the
         * input buffer. This will not read bytes that have not been
         * written.
         * 
         * This will mark itself as empty once it has been read.
         */
        uint32_t Read(uint8_t* buffer, uint32_t size);

        /**
         * \brief Sets the metadata information
         * \param destport the intended destination of this information
         * \param seqno the sequence number of the data
         * \param msg_type the type:'m' for message, 'd' for data, '\O' for none
         * 
         * this is stored here for encapsulation
         */
        void SetMetadata(uint16_t dest_port, uint32_t seqno, char msg_type);

    private:
        // Linked List
        RdmiNode*       m_next;             // a pointer to the next node

        // Storing Datagrams
        uint32_t        m_datagram_len;     // how many bytes are in the datagram (this can fill up multiple node buffers)
                                            // this only holds a value when its a header, otherwise len=0

        // Buffer Manipulation
        uint8_t*        m_buffer;           // internal data buffer for the node
        uint32_t        m_count;            // the number of bytes used
        uint32_t        m_cap;              // the number of bytes allocated

        // meta data
        // TODO: combine dest_port and message type
        uint16_t        m_dest_port;        // destination that this data is intended for
        uint32_t        m_seqno;            // sequence number of the data for the transmission    
        char            m_msg_type;         // message type 'm' for message, 'd' for data, \O for none
        

};

} // namespace rdmi
} // namespace ns3

#endif /* RDMI_NODE_H */
