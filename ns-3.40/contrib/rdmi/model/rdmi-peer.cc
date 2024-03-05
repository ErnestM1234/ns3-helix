

#include "ns3/log.h"

// RDMI
#include "rdmi-peer.h"
#include "rdmi-header.h"


namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(RDMIPeer);
NS_LOG_COMPONENT_DEFINE("RDMI_Peer");

TypeId RDMIPeer::GetTypeId()
{
  static TypeId tid =
    TypeId("ns3::RDMIPeer")
        .SetParent<Socket>()
    ;
  return tid;
}

RDMIPeer::RDMIPeer()
{
}

RDMIPeer::~RDMIPeer()
{
}

void RDMIPeer::SetRdmi(Ptr<RDMI> rdmi) {
  NS_LOG_FUNCTION(this << rdmi);
  m_rdmi = rdmi;
}



/* **************************************************************** */
/*  PEER SOCKET MANAGEMENT METHODS */
/* **************************************************************** */

int RDMIPeer::Bind(const Address& address)
{
  NS_LOG_FUNCTION(this << address);

  return m_peerSocket->Bind(address);
}

int RDMIPeer::Connect(const Address& address)
{
  NS_LOG_FUNCTION(this << address);

  return m_peerSocket->Connect(address);
}

int RDMIPeer::Listen()
{
  NS_LOG_FUNCTION(this);

  return m_peerSocket->Listen();
}

int RDMIPeer::Close()
{
  NS_LOG_FUNCTION(this);

  // TODO: notify any RDMISockets of closure

  return m_peerSocket->Close();
}


/* ******************************** */
/*  Private Methods */
/* ******************************** */

uint32_t RDMIPeer::GetTotalRxAvailable() const
{
  NS_LOG_FUNCTION(this);
  return m_peerSocket->GetRxAvailable();
}

uint32_t RDMIPeer::GetTotalTxAvailable() const
{
  NS_LOG_FUNCTION(this);
  return m_peerSocket->GetTxAvailable();
}

Ptr<Socket> RDMIPeer::GetPeerSocket() const
{
  NS_LOG_FUNCTION(this);
  return m_peerSocket;
}


/* **************************************************************** */
/*  RDMI SOCKET MANAGEMENT METHODS */
/* **************************************************************** */


/* ******************************** */
/*  Public Methods */
/* ******************************** */

void RDMIPeer::AddRdmiSocket (Ptr<RDMISocket> rdmi_socket)
{
  NS_LOG_FUNCTION(this << rdmi_socket);
  m_rdmiSocketList.insert(std::make_pair(rdmi_socket->GetPort(), rdmi_socket));
}

void RDMIPeer::RemoveRdmiSocket (Ptr<RDMISocket> rdmi_socket)
{
  NS_LOG_FUNCTION(this << rdmi_socket);
  m_rdmiSocketList.erase(rdmi_socket->GetPort());
}


void RDMIPeer::RemoveRdmiSocket (uint16_t id)
{
  NS_LOG_FUNCTION(this << id);
  m_rdmiSocketList.erase(id);
}

/* ******************************** */
/*  Private Methods */
/* ******************************** */

Ptr<RDMISocket> RDMIPeer::GetRdmiSocket(uint16_t id)
{
  NS_LOG_FUNCTION(this << id);
  return m_rdmiSocketList.find(id)->second;
}

size_t RDMIPeer::GetRdmiSocketCount() const
{
  return m_rdmiSocketList.size();
}

/* **************************************************************** */
/* RDMI SOCKET INTERFACE METHODS */
/* **************************************************************** */


/* ******************************** */
/*  Public Methods */
/* ******************************** */

int RDMIPeer::Send(uint16_t id, Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this << id << p);

  // get packet size
  uint32_t packet_size = p->GetSize();

  // make sure there space in socket buffer
  NS_ASSERT(1);

  // Copy packet
  // This should be a cheap operation because it uses virtual memory and COW
  // Also, not 100% sure this PeekPointer method is okay, but will have to do for now 
  Ptr<Packet> p_copy = new Packet(*PeekPointer(p));

  // add to socket buffer
  WriteToSocketMsgBuffer(id, p_copy);
  // TODO: data buffer as well

  // return the packet size to show that has been stored in buff
  return packet_size;
}

int RDMIPeer::Send(uint16_t id, Ptr<Packet> p, uint32_t flags)
{
  NS_LOG_FUNCTION(this << id << p << flags);
  return this->Send(id, p);
}

Ptr<Packet> RDMIPeer::Recv(uint16_t id, uint32_t maxSize, uint32_t flags)
{
  NS_LOG_FUNCTION(this << id << maxSize << flags);
  return m_peerSocket->Recv(maxSize, flags);
}


uint32_t RDMIPeer::GetTxAvailable(uint16_t id) const
{
    NS_LOG_FUNCTION(this << id);

    // TODO: implement prebuffering

    return GetTotalTxAvailable() / GetRdmiSocketCount();
}  


uint32_t RDMIPeer::GetRxAvailable(uint16_t id) const
{
    NS_LOG_FUNCTION(this << id);

     // TODO: implement this

    return 0; 
}  


/* **************************************************************** */
/* RDMI SOCKET ENCAPSULATION METHODS */
/* **************************************************************** */


/* ******************************** */
/*  Private Methods */
/* ******************************** */

void RDMIPeer::Encapsulate(Ptr<Packet> p, uint16_t dest_port, uint32_t seqno, char msg_type)
{
  NS_LOG_FUNCTION(this << p << dest_port << seqno << msg_type);


  // instantiate header from destination port
  RDMIHeader rdmi_header;
  // TO DO: add msg_type and seqno
  rdmi_header.SetDestinationPort(dest_port);

  // add header to packet
  p->AddHeader(rdmi_header);

}

uint32_t RDMIPeer::Decapsulate(Ptr<Packet> p, RDMIHeader h)
{
  NS_LOG_FUNCTION(this << p << h);

  // decapsulate packet
  return p->RemoveHeader(h);
}



/* **************************************************************** */
/* RDMI SOCKET ENCAPSULATION METHODS */
/* **************************************************************** */


/* ******************************** */
/*  Private Methods */
/* ******************************** */

Ptr<RdmiBuffer>
RDMIPeer::GetMessageBuffer(uint16_t id)
{
  NS_LOG_FUNCTION(this << id);
  return m_rdmiSocketMsgBuffers.find(id)->second;
}

Ptr<RdmiBuffer>
RDMIPeer::GetDataBuffer(uint16_t id)
{
  NS_LOG_FUNCTION(this << id);
  return m_rdmiSocketDataBuffers.find(id)->second;
}

void
RDMIPeer::WriteToSocketMsgBuffer(uint16_t id, Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this << id << p);
  
  // get a copy of the buffer
  uint32_t packet_size = p->GetSize();
  uint8_t *buffer = new uint8_t[packet_size]; // TODO: is there a faster way of doing this? (only 1 copy??)
  p->CopyData(buffer, packet_size);

  // write data to buffer
  Ptr<RdmiBuffer> rdmi_buffer = GetMessageBuffer(id);
  rdmi_buffer->WriteDatagram(buffer, packet_size);
}

void
RDMIPeer::WriteToSocketDataBuffer(uint16_t id, Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this << id << p);
  
  // make sure size of data can fit
  uint32_t packet_size = p->GetSize();

  // write data to buffer
  Ptr<RdmiBuffer> rdmi_buffer = GetDataBuffer(id);
  rdmi_buffer->WritePacket(p);
}

} // namespace ns3