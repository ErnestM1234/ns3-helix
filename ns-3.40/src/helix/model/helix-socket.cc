
// 1) try to implement UPD socket and send simple info
// 2) move to HELIX

#include "helix-socket.h"

#include "ns3/boolean.h"
#include "ns3/integer.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("HelixSocket");

NS_OBJECT_ENSURE_REGISTERED(HelixSocket);

TypeId
HelixSocket::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::HelixSocket")
            .SetParent<Socket>()
            .SetGroupName("Internet");
    return tid;
}

HelixSocket::HelixSocket()
{
    NS_LOG_FUNCTION(this);
}

HelixSocket::~HelixSocket()
{
    NS_LOG_FUNCTION(this);
}

} // namespace ns3