
// RDMI
#include "rdmi.h"

namespace ns3 {
namespace rdmi {

NS_OBJECT_ENSURE_REGISTERED(RDMI);
NS_LOG_COMPONENT_DEFINE("RDMI");

TypeId RDMI::GetTypeId()
{
  static TypeId tid = TypeId("ns3::RDMI")
    .SetParent<Application>()
    .AddConstructor<RDMI> ()
    ;
  return tid;
}

RDMI::RDMI()
{
}

RDMI::~RDMI()
{
}

void RDMI::DoDispose()
{
    NS_LOG_FUNCTION_NOARGS();
    // TODO: Set other vars to nullptr and dispose of them
    // chain up
    Application::DoDispose();
}

/* **************************************************************** */
/*  PRIVATE METHODS */
/* **************************************************************** */

void RDMI::StartApplication()
{
    NS_LOG_FUNCTION_NOARGS();
    // TODO: Start Application Logic
}

void RDMI::StopApplication()
{
    NS_LOG_FUNCTION_NOARGS();
    // TODO: Start Application Logic
}


} // namespace rdmi
} // namespace ns3
