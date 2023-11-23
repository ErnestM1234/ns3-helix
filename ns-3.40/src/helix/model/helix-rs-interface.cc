


#include "helix-rs-interface.h"
#include "ns3/helix-socket-impl.h"



#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"



namespace ns3
{

NS_LOG_COMPONENT_DEFINE("HelixRsInterface");

NS_OBJECT_ENSURE_REGISTERED(HelixRsInterface);


TypeId
HelixRsInterface::GetTypeId()
{
    static TypeId tid = TypeId("ns3::HelixRsInterface")
                            .SetParent<Object>()
                            .SetGroupName("Internet")
                            .AddConstructor<HelixRsInterface>();
    return tid;
}

HelixRsInterface::HelixRsInterface()
{
    NS_LOG_FUNCTION(this);
}

HelixRsInterface::~HelixRsInterface()
{
    NS_LOG_FUNCTION(this);
}

/* -------------------- Basic Socket Interface -------------------- */

int
HelixRsInterface::Bind(const Address & address)
{
    NS_LOG_FUNCTION(this << address);

    helix_rs_bind();
    return 0;
}

int
HelixRsInterface::Connect(const Address & address)
{
    NS_LOG_FUNCTION(this << address);

    helix_rs_connect();
    return 0;
}

int
HelixRsInterface::Listen()
{
    NS_LOG_FUNCTION(this);

    helix_rs_listen();
    return 0;
}

Ptr<Packet>
HelixRsInterface::Recv(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this);

    FFISharedBuffer buff = ConvertPacketToFFIBuff(p);
    FFISharedBuffer decoded_buff = helix_rs_recv(buff);
    Ptr<Packet> decoded_packet = ConvertFFIBuffToPacket(decoded_buff);
    // return decoded_packet;

    // TODO: remove this part and uncomment line above
    return p;
}

int
HelixRsInterface::Send(Ptr<Packet>)
{
    NS_LOG_FUNCTION(this);

    helix_rs_send(FFISharedBuffer());
    return 0;
}

int
HelixRsInterface::Close()
{
    NS_LOG_FUNCTION(this);

    helix_rs_close();
    return 0;
}


/* -------------------- Packet Manipulation -------------------- */

Ptr<Packet>
HelixRsInterface::EncodePacket(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << p);

    return p;
}

Ptr<Packet>
HelixRsInterface::DecodePacket(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << p);

    return p;
}

Ptr<Packet>
HelixRsInterface::ConvertFFIBuffToPacket(FFISharedBuffer b)
{
    NS_LOG_FUNCTION(this);

    return Ptr<Packet>();
}

FFISharedBuffer
HelixRsInterface::ConvertPacketToFFIBuff(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << p);

    return FFISharedBuffer();
}


}  // namespace ns3