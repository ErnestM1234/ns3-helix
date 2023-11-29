#ifndef HELIX_HELPER_H
#define HELIX_HELPER_H

#include "../model/helix.h"
#include "../model/helix-l4-protocol.h"
#include "ns3/core-module.h"
#include "ns3/node.h"


namespace ns3
{


class Node;

class HelixL4Protocol;

// Each class should be documented using Doxygen,
// and have an \ingroup helix directive

/* ... */

class HelixStackHelper
{
  public:
    HelixStackHelper() {}
    ~HelixStackHelper() {}

    // Adds given helix l4 protocol to a node
    void AddHelix(Ptr<Node> node, Ptr<HelixL4Protocol> helix);

    // Adds helix l4 protocol to a node
    void AddHelix(Ptr<Node> node);
};

} // namespace ns3

#endif /* HELIX_HELPER_H */
