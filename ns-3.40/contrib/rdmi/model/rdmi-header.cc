



#include "ns3/log.h"

// RDMI
#include "rdmi-header.h"


namespace ns3 {
namespace rdmi {

NS_OBJECT_ENSURE_REGISTERED(RDMIHeader);
NS_LOG_COMPONENT_DEFINE("RDMI_Header");


TypeId RDMIHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::RDMIHeader")
                            .SetParent<Header>()
                            .AddConstructor<RDMIHeader>();
    return tid;
}

RDMIHeader::RDMIHeader()
{
}

RDMIHeader::~RDMIHeader()
{
}



/* **************************************************************** */
/* OVERRIDDEN METHODS */
/* **************************************************************** */

TypeId RDMIHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}


void RDMIHeader::Print(std::ostream& os) const
{
    NS_LOG_FUNCTION(this);
    os << "destination port=" << m_destinationPort;
}

uint32_t RDMIHeader::GetSerializedSize() const
{
    NS_LOG_FUNCTION(this);
    // ports are two bytes
    return 2;
}

void RDMIHeader::Serialize(Buffer::Iterator start) const
{
    NS_LOG_FUNCTION(this);

    // we can serialize two bytes at the start of the buffer.
    // we write them in network byte order.
    start.WriteHtonU16(m_destinationPort);
    start.WriteHtonU32(m_seqno);
    start.WriteU8(m_msg_type); // TODO: will this just be casted from 1 -> 8 bytes?
}


uint32_t RDMIHeader::Deserialize(Buffer::Iterator start)
{
    NS_LOG_FUNCTION(this);

    // we can deserialize two bytes from the start of the buffer.
    // we read them in network byte order and store them
    // in host byte order.
    m_destinationPort = start.ReadNtohU16();

    // we return the number of bytes effectively read.
    return 2;
}


/* **************************************************************** */
/* CUSTOM METHODS */
/* **************************************************************** */

uint16_t
RDMIHeader::GetDestinationPort() const
{
    NS_LOG_FUNCTION(this);
    return m_destinationPort;
}

uint16_t
RDMIHeader::GetSeqno() const
{
    NS_LOG_FUNCTION(this);
    return m_seqno;
}

uint16_t
RDMIHeader::GetMsgType() const
{
    NS_LOG_FUNCTION(this);
    return m_msg_type;
}

void
RDMIHeader::SetDestinationPort(uint16_t port)
{
    NS_LOG_FUNCTION(this << port);
    m_destinationPort = port;
}

void
RDMIHeader::SetSeqno(uint32_t seqno)
{
    NS_LOG_FUNCTION(this << seqno);
    m_seqno = seqno;
}

void
RDMIHeader::SetMsgType(uint16_t msg_type)
{
    NS_LOG_FUNCTION(this << msg_type);
    m_msg_type = msg_type;
}

} // namespace rdmi
} // namespace ns3