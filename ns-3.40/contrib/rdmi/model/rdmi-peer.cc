

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

void RDMIPeer::SocketSendToMsgBuff(uint16_t id, Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this << id << p);

  // Copy packet
  // This should be a cheap operation because it uses virtual memory and COW
  // Also, not 100% sure this PeekPointer method is okay, but will have to do for now 
  Ptr<Packet> p_copy = new Packet(*PeekPointer(p));

  // add to socket buffer
  WriteToMsgBuffer(id, p_copy);
}

void RDMIPeer::SocketSendToDataBuff(uint16_t id, Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this << id << p);

  // Copy packet
  // TODO: Same issue here
  Ptr<Packet> p_copy = new Packet(*PeekPointer(p));

  // add to socket buffer
  WriteToMsgBuffer(id, p_copy);
}


Ptr<Packet> RDMIPeer::Recv(uint16_t id, uint32_t maxSize, uint32_t flags)
{
  NS_LOG_FUNCTION(this << id << maxSize << flags);
  return m_peerSocket->Recv(maxSize, flags);
}


uint32_t RDMIPeer::GetRxAvailable(uint16_t id) const
{
    NS_LOG_FUNCTION(this << id);

     // TODO: implement this

    return 0; 
}  


uint32_t RDMIPeer::GetTxAvailableMsgBuff(uint16_t id) const
{
    NS_LOG_FUNCTION(this << id);
    return GetMsgBuffer(id)->GetNodeCapacity();
}


uint32_t RDMIPeer::GetTxAvailableDataBuff(uint16_t id) const
{
    NS_LOG_FUNCTION(this << id);
    return GetDataBuffer(id)->GetNodeCapacity();
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

uint16_t
RDMIPeer::GetMsgBufferId(uint16_t id) const
{
  NS_LOG_FUNCTION(this << id);
  return id | (1 << 15);
}

uint16_t
RDMIPeer::GetDataBufferId(uint16_t id) const
{
  NS_LOG_FUNCTION(this << id);
  return id;
}

Ptr<RdmiBuffer>
RDMIPeer::GetMsgBuffer(uint16_t id) const
{
  NS_LOG_FUNCTION(this << id);
  return m_socketBuffers.find(GetMsgBufferId(id))->second;
}


Ptr<RdmiBuffer>
RDMIPeer::GetDataBuffer(uint16_t id) const
{
  NS_LOG_FUNCTION(this << id);
  return m_socketBuffers.find(GetDataBufferId(id))->second;
}


bool
RDMIPeer::CanCreateBufferPair()
{
  NS_LOG_FUNCTION(this);
  return MSG_BUFF_SIZE + DATA_BUFF_SIZE <= m_tx_buffer->GetNodeAvailable();
}


void
RDMIPeer::CreateBufferPair(uint16_t id)
{
  NS_LOG_FUNCTION(this << id);

  // instantiate
  Ptr<RdmiBuffer> msg_buffer = CreateObject<RdmiBuffer>();
  Ptr<RdmiBuffer> data_buffer = CreateObject<RdmiBuffer>();

  // transmission buffer donates
  NS_ASSERT(CanCreateBufferPair());
  m_tx_buffer->DonateNodes(msg_buffer, MSG_BUFF_SIZE);
  m_tx_buffer->DonateNodes(data_buffer, DATA_BUFF_SIZE);

  // add buffers to list
  m_socketBuffers.insert(std::make_pair(
    GetMsgBufferId(id),
    msg_buffer));
  m_rdmiSocketList.insert(std::make_pair(
    GetDataBufferId(id),
    data_buffer));
}


void
RDMIPeer::RemoveBufferPair(uint16_t id)
{
  NS_LOG_FUNCTION(this << id);

  // get buffers
  Ptr<RdmiBuffer> msg_buffer = GetMsgBuffer(id);
  Ptr<RdmiBuffer> data_buffer = GetDataBuffer(id);

  // clear all data (O(number of nodes) time, each node is constant)
  // TODO: is clearing necessary? can't this be done with good mem management?
  msg_buffer->Clear();
  data_buffer->Clear();

  // donate nodes to transmission buffer
  m_tx_buffer->DonateNodes(msg_buffer, msg_buffer->GetNodeCapacity());
  m_tx_buffer->DonateNodes(data_buffer, data_buffer->GetNodeCapacity());
}


void
RDMIPeer::WriteToMsgBuffer(uint16_t id, Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this << id << p);

  // get buffer
  Ptr<RdmiBuffer> rdmi_buffer = GetMsgBuffer(id);
  
  // make sure size of data can fit
  NS_ASSERT(p->GetSize() <= rdmi_buffer->GetCapacityBytes());

  // write to message buffer
  rdmi_buffer->WritePacket(p);
}

void
RDMIPeer::WriteToDataBuffer(uint16_t id, Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this << id << p);
  
  // get buffer
  Ptr<RdmiBuffer> rdmi_buffer = GetDataBuffer(id);

  // make sure size of data can fit
  NS_ASSERT(p->GetSize() <= rdmi_buffer->GetCapacityBytes());

  // write to data buffer
  rdmi_buffer->WritePacket(p);
}

} // namespace ns3