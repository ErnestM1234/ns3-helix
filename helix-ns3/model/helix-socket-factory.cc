
#include "helix-socket-factory.h"

#include "ns3/uinteger.h"

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(HelixSocketFactory);

TypeId
HelixSocketFactory::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::HelixSocketFactory").SetParent<SocketFactory>().SetGroupName("Internet");
    return tid;
}

} // namespace ns3
