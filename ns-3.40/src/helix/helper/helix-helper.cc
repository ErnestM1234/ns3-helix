#include "helix-helper.h"
#include "ns3/helix-socket-factory-impl.h"
#include "ns3/node.h"
#include "ns3/assert.h"

namespace ns3
{

void
HelixStackHelper::AddHelix(Ptr<Node> node, Ptr<HelixL4Protocol> helix)
{
    NS_ASSERT(node);
    std::cout << "Add Helix" << std::endl;
    std::cout << node->GetId() << std::endl;

    // aggregate helix l4 protocol into node
    if (!helix) helix = CreateObject<HelixL4Protocol>();
    helix->SetNode(node);
    node->AggregateObject(helix);

    // aggregate udp l4 protocol into helix l4 protocol
    // Ptr<UdpL4Protocol> udp = node->GetObject<UdpL4Protocol>();
    // helix->SetUdp(udp);
    // helix->AggregateObject(udp);
    // udp->AggregateObject(helix);

    // aggregate helix socket factory impl into node
    Ptr<HelixSocketFactoryImpl> helixFactoryImpl = CreateObject<HelixSocketFactoryImpl>();
    helixFactoryImpl->SetHelix(helix);
    node->AggregateObject(helixFactoryImpl);
}

void
HelixStackHelper::AddHelix(Ptr<Node> node)
{
    NS_ASSERT(node);

    Ptr<HelixL4Protocol> helix = CreateObject<HelixL4Protocol>();
    AddHelix(node, helix);
}

} // namespace ns3
