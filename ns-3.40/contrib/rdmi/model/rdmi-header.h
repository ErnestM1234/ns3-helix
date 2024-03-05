#ifndef RDMI_HEADER_H
#define RDMI_HEADER_H

#include "ns3/header.h"
#include "ns3/ptr.h"


namespace ns3
{

class Packet;

// TODO: Add sequence number


/**
 * \ingroup rdmi
 * \defgroup rdmiheader RdmiHeader
 */

/**
 * \ingroup rdmiheader
 *
 * \brief Header for rdmi datagrams
 *
 * This is a header used for multiplexing the dataflows between
 * the peer socket and the rdmi sockets. Using buffers were
 * chosen over using tags because buffers should represent on
 * the wire data.
 * 
 * I have not added a checksum because I probably won't
 * get around to doing that.
 * 
 * TODO: is it okay that header is only of size 2 bytes,
 * or should it be padded to 8?
 * 
 * Example: ns-3.40/src/network/examples/main-packet-header.cc
 */

class RDMIHeader : public Header {
    public:
        /**
         * \brief Get the type ID.
         * \return the object TypeId
         */
        static TypeId GetTypeId();
        RDMIHeader();
        ~RDMIHeader() override;

        /* **************************************************************** */
        /* OVERRIDDEN METHODS */
        /* **************************************************************** */
    
    private:

        TypeId GetInstanceTypeId() const override;

        /**
         * \returns the expected size of the header.
         *
         * This method is used by Packet::AddHeader
         * to store a header into the byte buffer of a packet. This method
         * should return the number of bytes which are needed to store
         * the full header data by Serialize.
         */
        uint32_t GetSerializedSize() const override;


        /**
         * \param start an iterator which points to where the header should
         *        be written.
         *
         * This method is used by Packet::AddHeader to
         * store a header into the byte buffer of a packet.
         * The data written
         * is expected to match bit-for-bit the representation of this
         * header in a real network.
         */
        void Serialize(Buffer::Iterator start) const override;

        /**
         * \param start an iterator which points to where the header should
         *        read from.
         * \returns the number of bytes read.
         *
         * This method is used by Packet::RemoveHeader to
         * re-create a header from the byte buffer of a packet.
         * The data read is expected to
         * match bit-for-bit the representation of this header in real
         * networks.
         *
         * Note that data is not actually removed from the buffer to
         * which the iterator points.  Both Packet::RemoveHeader() and
         * Packet::PeekHeader() call Deserialize(), but only the RemoveHeader()
         * has additional statements to remove the header bytes from the
         * underlying buffer and associated metadata.
         */
        uint32_t Deserialize(Buffer::Iterator start) override;

        /**
         * \param os output stream
         * This method is used by Packet::Print to print the
         * content of a header as ascii data to a c++ output stream.
         * Although the header is free to format its output as it
         * wishes, it is recommended to follow a few rules to integrate
         * with the packet pretty printer: start with flags, small field
         * values located between a pair of parens. Values should be separated
         * by whitespace. Follow the parens with the important fields,
         * separated by whitespace.
         * i.e.: (field1 val1 field2 val2 field3 val3) field4 val4 field5 val5
         */
        void Print(std::ostream& os) const override;



        /* **************************************************************** */
        /* CUSTOM METHODS */
        /* **************************************************************** */

    public: 

        /**
         * \brief Get the destination port
         * \return the destination port for this RdmiHeader
         */
        uint16_t GetDestinationPort() const;

        /**
         * \brief Get the sequence number
         * \return the sequence number
         */
        uint16_t GetSeqno() const;

        /**
         * \brief Get the message type
         * \return the message type
         * 
         * message type 'm' for message, 'd' for data, \O for none
         */
        uint16_t GetMsgType() const;

        /**
         * \brief Set the destination port
         * \param port the destination port for this RdmiHeader
         */
        void SetDestinationPort(uint16_t port);

        /**
         * \brief Set the destination port
         * \param seqno sequence number of the data for the transmission
         */
        void SetSeqno(uint32_t seqno);

        /**
         * \brief Set the message type
         * \param msg_type message type
         * 
         * message type 'm' for message, 'd' for data, \O for none
         */
        void SetMsgType(uint16_t msg_type);


    private:
        // assumption: destination port is enough information to
        // map to an RDMI Socket
        uint16_t    m_destinationPort{0xfffd};  // Destination port
        uint32_t    m_seqno;                    // sequence number of the data for the transmission    
        char        m_msg_type;                 // message type 'm' for message, 'd' for data, \O for none

};


} // namespace ns3

#endif /* RDMI_HEADER_H */