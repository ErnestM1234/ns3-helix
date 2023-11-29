
#ifndef HELIX_SOCKET_H
#define HELIX_SOCKET_H


#include "ns3/socket.h"


namespace ns3
{

/**
 * \ingroup socket
 *
 * \brief (abstract) a socket for HELIX protocol
 *
 * This class exists solely for hosting HELIX Socket attributes that can
 * be reused across different implementations.
 */

class HelixSocket : public Socket
{
  public:
    /**
     * Get the type ID.
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    HelixSocket();
    ~HelixSocket() override;

};

} // namespace ns3

#endif /* HELIX_SOCKET_H */