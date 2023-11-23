



#ifndef HELIX_SOCKET_FACTORY_IMPL_H
#define HELIX_SOCKET_FACTORY_IMPL_H

#include "helix-socket-factory.h"

#include "ns3/internet-module.h"

#include "ns3/ptr.h"

namespace ns3
{

class HelixL4Protocol;

/**
 * \ingroup socket
 * \ingroup helix
 *
 * \brief socket factory implementation for native ns-3 HELIX
 *
 *
 * This class serves to create sockets of the HelixSocketBase type.
 */
class HelixSocketFactoryImpl : public HelixSocketFactory
{
  public:
    HelixSocketFactoryImpl();
    ~HelixSocketFactoryImpl() override;

    /**
     * \brief Set the associated HELIX L4 protocol.
     * \param helix the HELIX L4 protocol
     */
    void SetHelix(Ptr<HelixL4Protocol> helix);

    Ptr<Socket> CreateSocket() override;

  protected:
    void DoDispose() override;

  private:
    Ptr<HelixL4Protocol> m_helix; //!< the associated HELIX L4 protocol
};

} // namespace ns3

#endif /* HELIX_SOCKET_FACTORY_IMPL_H */
