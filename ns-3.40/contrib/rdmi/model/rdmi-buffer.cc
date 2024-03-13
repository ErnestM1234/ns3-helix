


#include <ns3/log.h>
#include <ns3/fatal-error.h>


// RDMI
#include "rdmi-buffer.h"


namespace ns3 {
namespace rdmi {

NS_OBJECT_ENSURE_REGISTERED(RdmiBuffer);
NS_LOG_COMPONENT_DEFINE("Rdmi_Buffer");

TypeId RdmiBuffer::GetTypeId()
{
  static TypeId tid =
    TypeId("ns3::RdmiBuffer")
        .SetParent<Object>()
    ;
  return tid;
}

RdmiBuffer::RdmiBuffer()
    :   m_node_count(0),
        m_byte_count(0)
{
}


RdmiBuffer::~RdmiBuffer()
{
}


/* **************************************************************** */
/* RDMI BUFFER DATA MANIPULATION METHODS */
/* **************************************************************** */

void
RdmiBuffer::WriteBuffer(uint8_t* buffer, uint32_t size)
{
    NS_LOG_FUNCTION(this << buffer << size);
    // no datagrams of size 0, this messes up the demarkation of header nodes
    NS_ASSERT(size <= m_byte_cap - m_byte_count && size != 0);

    // get the number of nodes we will have to visit (nodes have all same capacity)
    uint32_t node_space = m_end->GetCapacity(); // total bytes in a node
    uint16_t node_count = (size / node_space) + 1; // integer div, so not counting overflow to last node
    uint32_t extra_bytes = size % node_space;

    // mark first node as a header node
    m_end = m_end->GetNext();
    m_end->SetDatagramLength(size);
    
    // write to nodes
    if (node_count == 1) {  // case: one node

        // write to the 1st node
        m_end->Write(buffer, extra_bytes);
        m_end->SetMetadata(m_dest_port, m_seqno, m_msg_type);
        m_seqno += 1;

    } else {    // case: multiple nodes

        // write to the 1st node
        m_end->Write(buffer, size);
        m_end->SetMetadata(m_dest_port, m_seqno, m_msg_type);
        m_seqno += 1;

        // write to nodes [2,n-1] (if they were to be one indexed)
        for (int i = 1; i < node_count-1; i++) {
            m_end = m_end->GetNext();
            int offset = i * node_space; // TODO: Check that this offset works
            m_end->Write(buffer + offset, node_space);
            m_end->SetMetadata(m_dest_port, m_seqno, m_msg_type);
            m_seqno += 1;
        }

        // write to final node
        m_end = m_end->GetNext();
        int offset = (node_count-1) * node_space; // TODO: Check that this offset works
        m_end->Write(buffer + offset, extra_bytes);
        m_end->SetMetadata(m_dest_port, m_seqno, m_msg_type);
        m_seqno += 1;
    }

    // update counts
    m_node_count += node_count;
    m_byte_count += node_count * node_space; // even if extra_bytes < node_space, entire node still used
}


void
RdmiBuffer::WritePacket(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << p);

    // Calc statistics
    uint32_t size = p->GetSize();
    NS_ASSERT(size <= m_byte_cap - m_byte_count && size != 0); // we assume we only write when buff empty
    uint32_t node_space = m_end->GetCapacity(); // total bytes in a node
    uint16_t node_count = (size / node_space) + 1; // integer div, so not counting overflow to last node

    // mark first node as a header node
    m_end = m_end->GetNext();
    m_end->SetDatagramLength(size);

    // write to nodes
    uint32_t bytesWritten = 0;
    if (node_count == 1) {  // case: one node

        // write to first node
        bytesWritten = m_end->WritePacket(p);
        m_end->SetMetadata(m_dest_port, m_seqno, m_msg_type);
        m_seqno += 1;
        // move packet start pointer
        p->RemoveAtStart(bytesWritten);

    } else {    // case: multiple nodes

        // write to the 1st node
        bytesWritten = m_end->WritePacket(p);
        m_end->SetMetadata(m_dest_port, m_seqno, m_msg_type);
        m_seqno += 1;
        // move packet start pointer
        p->RemoveAtStart(bytesWritten);

        // write to nodes [2,n-1] (if they were to be one indexed)
        for (int i = 1; i < node_count-1; i++) {
            m_end = m_end->GetNext();
            bytesWritten = m_end->WritePacket(p);
            m_end->SetMetadata(m_dest_port, m_seqno, m_msg_type);
            m_seqno += 1;
            // move packet start pointer
            p->RemoveAtStart(bytesWritten);
        }

        // write to final node
        m_end = m_end->GetNext();
        bytesWritten = m_end->WritePacket(p);
        m_end->SetMetadata(m_dest_port, m_seqno, m_msg_type);
        m_seqno += 1;
        // move packet start pointer
        p->RemoveAtStart(bytesWritten);
    }

    // Sanity check: the packet should be empty now
    NS_ASSERT(p->GetSize() == 0);

    // update counts
    m_node_count += node_count;
    m_byte_count += node_count * node_space; // even if extra_bytes < node_space, entire node still used
}

void
RdmiBuffer::Write(uint8_t* buffer, uint32_t size)
{
    NS_LOG_FUNCTION(this << buffer << size);
    WriteBuffer(buffer, size);
}

void
RdmiBuffer::ReadDatagram(uint8_t* buffer)
{
    NS_LOG_FUNCTION(this << buffer);
    // get first filled node
    m_start = m_start->GetNext();

    // calculate useful values
    uint32_t datagram_size = m_start->GetDatagramLength(); // how many bytes the datagram is
    uint32_t node_space = m_start->GetCapacity();    // the number of bytes in a node
    uint16_t node_count = (datagram_size / node_space) + 1; // integer div, so not counting overflow to last node
    uint32_t extra_bytes = datagram_size % node_space;
    NS_ASSERT(datagram_size <= m_byte_count && datagram_size != 0); // only reads datagram from a datagram header node

    // read data to buffer
    if (node_count == 1) { // case: only one node
        m_start->Read(buffer, extra_bytes); // read and reset node
    } else { // case: many nodes
        for (int i = 0; i < node_count - 1; i++) {
            int offset = i * node_space; // TODO: Check that this offset works
            m_start->Read(buffer + offset, extra_bytes); // read and reset node
            m_start = m_start->GetNext();
        }
        // Sanity check: make sure that we are not overwriting a header
        NS_ASSERT(m_start->GetDatagramLength() == 0);
        int offset = (node_count - 1) * node_space;
        m_start->Read(buffer + offset, extra_bytes); // read and reset node
    }

    // update counts
    m_byte_count -= node_count * node_space;
    m_node_count -= node_count;
}


uint16_t
RdmiBuffer::GetSizeBytes() const
{
    NS_LOG_FUNCTION(this);
    return m_byte_count;
}

uint16_t
RdmiBuffer::GetCapacityBytes() const
{
    NS_LOG_FUNCTION(this);
    return m_byte_cap;
}

void
RdmiBuffer::SetCapacityBytes(uint32_t capacity)
{
    NS_LOG_FUNCTION(this << capacity);
    m_byte_cap = capacity;
}

uint16_t
RdmiBuffer::GetAvailableDatagramSize() const
{
    NS_LOG_FUNCTION(this);
    if (m_start) {
        // TODO: if there are concurrent read writes, may become an issue
        // this could get written in before the rest of the datagram has
        // been fully copied
        return m_start->GetDatagramLength();
    }
    return 0;
}

void
RdmiBuffer::Clear()
{
    NS_LOG_FUNCTION(this);
    RdmiNode* curr = m_start;
    for (int i = 0; i < m_node_cap; i++) {
        curr->Reset();
        curr = curr->GetNext();
    }
}

/* **************************************************************** */
/* RDMI NODE MANIPULATION METHODS */
/* **************************************************************** */

void
RdmiBuffer::AllocateNodes()
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(m_node_cap <= RDMI_BUFFER_MAX_NODES);
    if (m_node_cap == 0) return;

    m_start = new RdmiNode(); // default size
    m_end = m_start;
    RdmiNode* nextNode;

    for (int i = 1; i < m_node_cap; i++) {
        nextNode = new RdmiNode(); // default size
        m_end->SetNext(nextNode);
        m_end = nextNode;
    }
}

void
RdmiBuffer::AllocateNodes(uint16_t size)
{
    NS_LOG_FUNCTION(this << size);
    NS_ASSERT(size <= RDMI_BUFFER_MAX_NODES);
    m_node_cap = size;
    AllocateNodes();
}

void
RdmiBuffer::DeallocateNodes()
{
    NS_LOG_FUNCTION(this);
    // TODO: Figure out how to deallocate
}


void
RdmiBuffer::SwapNodes(Ptr<RdmiBuffer> target, uint16_t size)
{
    NS_LOG_FUNCTION(this << target << size);

    // target: enough full nodes to swap
    NS_ASSERT(size <= target->GetNodeCount());
    // recipient: enough empty nodes to swap
    NS_ASSERT(size <= m_node_cap - m_node_count);

    // get some helpful pointers
    RdmiNode* emptStart = m_end->GetNext();
    RdmiNode* fullStart = target->GetStartNode()->GetNext();
    NS_ASSERT(emptStart && fullStart); // sanity check

    // get the end node of each linked list
    RdmiNode* emptEnd = emptStart;
    RdmiNode* fullEnd = fullStart;
    for (int i = 1; i < size; i++) { // start at i=1
        emptEnd = emptEnd->GetNext();
        fullEnd = fullEnd->GetNext();
        NS_ASSERT(emptEnd && fullEnd); // sanity check
    }

    // get nodes after the ones that are to be swapped
    RdmiNode* emptEndNext = emptEnd->GetNext();
    RdmiNode* fullEndNext = fullEnd->GetNext();
    NS_ASSERT(emptEndNext && fullEndNext); // sanity check

    // update target
    uint32_t node_space = m_start->GetCapacity(); // the number of bytes in a node
    target->GetStartNode()->SetNext(emptStart);             // loop in start of buffer
    target->SetStartNode(emptEnd);                          // update start pointer
    target->GetStartNode()->SetNext(fullEndNext);           // loop in end of buffer
    target->SetNodeCount(target->GetNodeCount() - size);    // update node count
    target->SetCapacityBytes(target->GetCapacityBytes() - size * node_space); // update byte count
    // update recipient
    m_end->SetNext(fullStart);          // loop in start of buffer
    m_end = fullEnd;                    // update start pointer
    m_end->SetNext(fullEndNext);        // loop in end of buffer
    m_node_count += size;               // update node count
    m_byte_cap += size * node_space;    // update byte count
}


void
RdmiBuffer::DonateNodes(Ptr<RdmiBuffer> target, uint16_t size)
{
    NS_LOG_FUNCTION(this << target << size);

    // Target: none
    // Donor: make sure there's enough empty nodes to donate
    NS_ASSERT(size <= m_node_cap - m_node_count);
    // Donor: make sure there's enough bytes to donate
    uint32_t node_space = m_start->GetCapacity(); // the number of bytes in a node
    NS_ASSERT(size * node_space <= m_byte_cap);

    // get some helpful pointers
    RdmiNode* emptStart = m_end->GetNext();
    RdmiNode* fullStart = target->GetStartNode()->GetNext();
    NS_ASSERT(emptStart && fullStart); // sanity check
    RdmiNode* emptEnd = emptStart;
    for (int i = 1; i < size; i++) { // start at i=1
        emptEnd = emptEnd->GetNext();
        NS_ASSERT(emptEnd); // sanity check
    }
    RdmiNode* emptEndNext = emptEnd->GetNext();
    NS_ASSERT(emptEndNext); // sanity check

    // update target
    target->GetStartNode()->SetNext(emptStart);             // loop in start of buffer
    target->SetStartNode(emptEnd);                          // update start pointer
    target->GetStartNode()->SetNext(fullStart);             // loop in end of buffer
    target->SetNodeCapacity(target->GetNodeCapacity() + size);    // update cap node count
    target->SetCapacityBytes(target->GetCapacityBytes() + size * node_space); // update byte count
    // update donor
    m_end->SetNext(emptEndNext);        // remove the bytes from the loop
    m_node_cap -= size;                 // update the size
    m_byte_cap -= size * node_space;    // update byte count
}

RdmiNode*
RdmiBuffer::GetStartNode() const
{
    NS_LOG_FUNCTION(this);
    return m_start;
}

RdmiNode*
RdmiBuffer::GetEndNode() const
{
    NS_LOG_FUNCTION(this);
    return m_end;
}


void
RdmiBuffer::SetStartNode(RdmiNode* start)
{
    NS_LOG_FUNCTION(this << start);
    m_start = start;
}

void
RdmiBuffer::SetEndNode(RdmiNode* end)
{
    NS_LOG_FUNCTION(this << end);
    m_end = end;
}

uint16_t
RdmiBuffer::GetNodeCount() const
{
    NS_LOG_FUNCTION(this);
    return m_node_count;
}

uint16_t
RdmiBuffer::GetNodeCapacity() const
{
    return m_node_cap;
}

uint16_t
RdmiBuffer::GetNodeAvailable() const
{
    return m_node_cap - m_node_count;
}


void
RdmiBuffer::SetNodeCount(uint16_t count)
{
    NS_LOG_FUNCTION(this << count);
    NS_ASSERT(count <= m_node_cap);
    m_node_count = count;
}

void
RdmiBuffer::SetNodeCapacity(uint16_t capacity)
{
    NS_LOG_FUNCTION(this << capacity);
    m_node_cap = capacity;
}

} // namespace rdmi
} // namespace ns3