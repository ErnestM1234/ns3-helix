#ifndef RDMI_H
#define RDMI_H


#include "ns3/application.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"

#include "rdmi-peer.h"
#include "rdmi-socket.h"


// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup rdmi TODO: Description of the rdmi
 */

namespace ns3 {
namespace rdmi {


// class ns3::Node;
// class ns3::Packet;
// typedef ns3::Node Node;
// typedef ns3::Packet Packet;

class RDMIPeer;
class RDMISocket;

/**
 * \ingroup rdmi
 *
 * \brief A reliable datagram multiplexor interface
 *
 * TODO: 1 paragraph description
 */
class RDMI : public Application {
    public:
        /**
         * \brief Get the type ID.
         * \return the object TypeId
         */
        static TypeId GetTypeId();
        RDMI();
        ~RDMI() override;

    protected:
        void DoDispose() override;

    private:
        void StartApplication() override; // TODO: Are Start and Stop necessary?
        void StopApplication() override;

    private:
        // std::vector<Ptr<RDMIPeer> >              m_peerList;        // A list of currently connected peers
        // std::vector<Ptr<RDMISocket> >            m_rdmiSocketList;  // A list of current rdmiSockets
};

} // namespace rdmi
} // namespace ns3

#endif /* RDMI_H */
