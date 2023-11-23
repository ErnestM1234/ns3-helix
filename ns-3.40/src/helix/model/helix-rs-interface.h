
/*
 * This is a wrapper class for the Rust implementation of
 * the HELIX protocol. The purpose of this class is for
 * accessing the benefits of the ns3 logging system.
 * 
 * Besides set up function calls, there should be a
 * one to one mapping between these calls and calls to 
 * rust functions.
 * 
 * This class shold also bridge the gap between the
 * rust functions and c++. It should handle all type
 * conversion etc.
 * 
 * 
*/
#ifndef HELIX_RS_INTERFACE_H
#define HELIX_RS_INTERFACE_H


#include "ns3/helix-socket-impl.h"

#include "/Users/ernestmccarter/Documents/Princeton/School/concentration/senior thesis/ns3/workspace/ns-allinone-3.40/helix-rs/helix_rs.h"

#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"


namespace ns3
{


class HelixSocketImpl;

/**
 * \ingroup helix
 *
 * \brief An FFI for HELIX in Rust
 *
 * This class provides an interface for a HELIX implementation
 * in rust. It interact by invoking Rust methods through an FFI.
 * 
 * Essentially, this is a wrapper class that provides a 1-to-1
 * mapping of C++ and Rust methods that offers two advantages:
 * (1) This provides ns3 logging for any HELIX functions.
 * (2) This handles all type conversions from all ns3 types to
 *      shared C++/Rust types used for interfacing.
 * This class is meant to reduce complexity and abstract the
 * overhead of interfacing between rust and C++.
 *
 */
class HelixRsInterface : public Object
{
    public:
        /**
         * \brief Get the type ID.
         * \return the object TypeId
         */
        static TypeId GetTypeId();

        /**
         * Create a HelixRsInterface object
         */
        HelixRsInterface();
        ~HelixRsInterface();

        /* -------------------- HELIX Interface -------------------- */
        int Bind(const Address& address);
        int Connect(const Address& address);
        int Send(Ptr<Packet> p);
        int Listen();
        Ptr<Packet> Recv(Ptr<Packet> p);
        int Close();

    private:

        /* -------------------- Packet Manipulation -------------------- */
        /**
         * \brief Add HELIX wrapper to a packet
         * \param  p - packet to encode
         * \returns Encoded packet
         */
        Ptr<Packet> EncodePacket(Ptr<Packet> p);
        /**
         * \brief Remove HELIX wrapper from a packet
         * \param  p - packet to decode
         * \returns Decoded packet
         */
        Ptr<Packet> DecodePacket(Ptr<Packet> p);
        /**
         * \brief Convert a packet to an FFISharedBuffer
         * \param  p - packet
         * \returns Buffer version of packet
         */
        FFISharedBuffer ConvertPacketToFFIBuff(Ptr<Packet> p);
        /**
         * \brief Convert an FFISharedBuffer to a packet
         * \param  b - buffer
         * \returns Packet version of buffer
         */
        Ptr<Packet> ConvertFFIBuffToPacket(FFISharedBuffer b);
         

        
        /* -------------------- Member Variables -------------------- */

};

} // namespace ns3


#endif /* HELIX_RS_INTERFACE_H */