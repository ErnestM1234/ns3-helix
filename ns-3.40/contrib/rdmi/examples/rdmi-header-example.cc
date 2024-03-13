#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/simulator.h"

// RDMI
#include "ns3/rdmi-header.h"


#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("RdmiHeaderExample");

int
main(int argc, char* argv[])
{
    // Enable the packet printing through Packet::Print command.
    Packet::EnablePrinting();

    // instantiate a header.
    rdmi::RDMIHeader sourceHeader;
    sourceHeader.SetDestinationPort(5555);

    // instantiate a packet
    Ptr<Packet> p = Create<Packet>();

    // and store my header into the packet.
    p->AddHeader(sourceHeader);

    // print the content of my packet on the standard output.
    p->Print(std::cout);
    std::cout << std::endl;

    // you can now remove the header from the packet:
    rdmi::RDMIHeader destinationHeader;
    p->RemoveHeader(destinationHeader);

    // and check that the destination and source
    // headers contain the same values.
    NS_ASSERT(sourceHeader.GetDestinationPort() == destinationHeader.GetDestinationPort());

    return 0;
}
