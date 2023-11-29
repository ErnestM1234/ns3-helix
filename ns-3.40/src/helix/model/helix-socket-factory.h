
#ifndef HELIX_SOCKET_FACTORY_H
#define HELIX_SOCKET_FACTORY_H

#include "ns3/socket-factory.h"

namespace ns3
{

class Socket;

/**
 * \ingroup socket
 * \ingroup helix
 *
 * \brief API to create HELIX socket instances
 *
 * This abstract class defines the API for HELIX sockets.
 * 
 * All HELIX socket factory implementations must provide an implementation
 * of CreateSocket
 * below, and should make use of the default values configured below.
 *
 * \see HelixSocketFactoryImpl
 *
 */
class HelixSocketFactory : public SocketFactory
{
  public:
    /**
     * Get the type ID.
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

};

} // namespace ns3

#endif /* HELIX_SOCKET_FACTORY_H */
