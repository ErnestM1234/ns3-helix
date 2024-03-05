

#include <ns3/log.h>
#include <ns3/fatal-error.h>

// RDMI
#include "rdmi-node.h"



namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Rdmi_Node");

RdmiNode::RdmiNode()
    :   m_count(0),
        m_cap(RDMI_BUFF_DEFAULT_SIZE)
{
    NS_LOG_FUNCTION(this << m_cap);
    m_buffer = new uint8_t[RDMI_BUFF_DEFAULT_SIZE];
}

RdmiNode::RdmiNode(uint32_t capacity)
    :   m_count(0),
        m_cap(capacity)
{
    NS_LOG_FUNCTION(this << capacity);
    NS_ASSERT(capacity <= RDMI_BUFF_MAX_SIZE);
    m_buffer = new uint8_t[capacity];
}

RdmiNode::~RdmiNode()
{
    NS_LOG_FUNCTION(this);
    delete[] m_buffer;
}

RdmiNode*
RdmiNode::GetNext()
{
    NS_LOG_FUNCTION(this);
    return m_next;
}

void
RdmiNode::SetNext(RdmiNode* next)
{
    NS_LOG_FUNCTION(this << next);
    m_next = next;
}

uint32_t
RdmiNode::GetDatagramLength()
{
    NS_LOG_FUNCTION(this);
    return m_datagram_len;
}

void
RdmiNode::SetDatagramLength(uint32_t len)
{
    NS_LOG_FUNCTION(this << len);
    m_datagram_len = len;
}


bool
RdmiNode::ContainsDatagramStart()
{
    NS_LOG_FUNCTION(this);
    return m_datagram_len != 0;
}

void
RdmiNode::Reset()
{
    NS_LOG_FUNCTION(this);
    m_datagram_len = 0;
    m_count = 0;
    m_dest_port = 0;
    m_seqno = 0;
    m_msg_type = '\0';
}

uint32_t
RdmiNode::GetSize() 
{
    NS_LOG_FUNCTION(this);
    return m_count;
}


uint32_t
RdmiNode::GetCapacity() const
{
    NS_LOG_FUNCTION(this);
    return m_cap;
}

uint32_t
RdmiNode::WritePacket(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << p);
    NS_ASSERT(m_count == 0); // we can only write to empty nodes

    // TODO: Potential issue: would writing include more data than just buffer contents??
    // get packet size
    uint32_t size = p->GetSize();
    NS_ASSERT(size != 0); // should not waste an extra node

    // write packet to buffer (only write what we have space for)
    uint32_t numBytes = std::min(size, m_cap - m_count);
    numBytes = p->CopyData(m_buffer, numBytes);
    
    // update the counts
    m_count += numBytes;

    return numBytes;
}

uint32_t
RdmiNode::Write(const uint8_t* buffer, uint32_t size)
{
    // NEW VERSION: Can only write once
    NS_LOG_FUNCTION(this << buffer << size);
    NS_ASSERT(size <= m_cap && m_count == 0); // we can only write to empty nodes
    

    // can only write up to the available amount of bytes
    uint32_t numBytes = std::min(size, m_cap - m_count);
    memcpy(m_buffer, buffer, numBytes); // TODO: does line should control for memcpy security?

    // update counts
    m_count += numBytes;

    return numBytes;

    // OLD VERSION: Can write multiple times
    // NS_LOG_FUNCTION(this << &buffer << size);
    // NS_ASSERT(m_cap - m_count >= 0); // sanity check

    // // can only write up to the available amount of bytes
    // uint32_t numBytes = std::min(size, m_cap - m_count);
    // uint8_t* to = m_buffer + m_count;

    // memcpy(to, buffer, numBytes);
    // m_count += numBytes;
    // return numBytes;
}

uint32_t
RdmiNode::Read(uint8_t* buffer, uint32_t size)
{
    NS_LOG_FUNCTION(this << &buffer << size);

    // can only read up to the available amount of bytes
    uint32_t numBytes = std::min(size, m_cap - m_count);
    memcpy(buffer, m_buffer, numBytes);

    // mark self as empty
    Reset();

    return numBytes;
}

void
RdmiNode::SetMetadata(uint16_t dest_port, uint32_t seqno, char msg_type)
{
    NS_LOG_FUNCTION(this << dest_port << seqno << msg_type);
    m_dest_port = dest_port;
    m_seqno = seqno;
    m_msg_type = msg_type;
}


} // namespace ns3