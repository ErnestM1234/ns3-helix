



#include "helix-socket-factory-impl.h"

#include "helix-l4-protocol.h"

#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/socket.h"

namespace ns3
{


NS_LOG_COMPONENT_DEFINE("HelixSocketFactoryImpl");

NS_OBJECT_ENSURE_REGISTERED(HelixSocketFactoryImpl);


HelixSocketFactoryImpl::HelixSocketFactoryImpl()
    : m_helix(nullptr)
{
    NS_LOG_FUNCTION(this);
}

HelixSocketFactoryImpl::~HelixSocketFactoryImpl()
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(!m_helix);
}

void
HelixSocketFactoryImpl::SetHelix(Ptr<HelixL4Protocol> helix)
{
    NS_LOG_FUNCTION(this);
    m_helix = helix;
}

Ptr<Socket>
HelixSocketFactoryImpl::CreateSocket()
{
    NS_LOG_FUNCTION(this);
    return m_helix->CreateSocket();
}

void
HelixSocketFactoryImpl::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_helix = nullptr;
    HelixSocketFactory::DoDispose();
}

} // namespace ns3
