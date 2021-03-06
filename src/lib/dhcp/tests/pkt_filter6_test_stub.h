// Copyright (C) 2014 Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#ifndef PKT_FILTER6_TEST_STUB_H
#define PKT_FILTER6_TEST_STUB_H

#include <asiolink/io_address.h>
#include <dhcp/iface_mgr.h>
#include <dhcp/pkt_filter6.h>
#include <dhcp/pkt6.h>

namespace bundy {
namespace dhcp {
namespace test {

/// @brief A stub implementation of the PktFilter6 class.
///
/// This class implements abstract methods of the @c bundy::dhcp::PktFilter6
/// class. It is used by unit tests, which test protected methods of the
/// @c bundy::dhcp::test::PktFilter6 class. The implemented abstract methods are
/// no-op.
class PktFilter6TestStub : public PktFilter6 {
public:

    /// @brief Constructor.
    PktFilter6TestStub();

    /// @brief Simulate opening of the socket.
    ///
    /// This function simulates opening a primary socket. In reality, it doesn't
    /// open a socket but the socket descriptor returned in the SocketInfo
    /// structure is always set to 0.
    ///
    /// @param iface An interface descriptor.
    /// @param addr Address on the interface to be used to send packets.
    /// @param port Port number to bind socket to.
    /// @param join_multicast A flag which indicates if the socket should be
    /// configured to join multicast (if true).
    ///
    /// @note All parameters are ignored.
    ///
    /// @return A SocketInfo structure with the socket descriptor set to 0. The
    /// fallback socket descriptor is set to a negative value.
    virtual SocketInfo openSocket(const Iface& iface,
                                  const bundy::asiolink::IOAddress& addr,
                                  const uint16_t port,
                                  const bool join_multicast);

    /// @brief Simulate reception of the DHCPv6 message.
    ///
    /// @param socket_info A structure holding socket information.
    ///
    /// @note All parameters are ignored.
    ///
    /// @return always a NULL object.
    virtual Pkt6Ptr receive(const SocketInfo& socket_info);

    /// @brief Simulates sending a DHCPv6 message.
    ///
    /// This function does nothing.
    ///
    /// @param iface An interface to be used to send DHCPv6 message.
    /// @param port A port used to send a message.
    /// @param pkt A DHCPv6 to be sent.
    ///
    /// @note All parameters are ignored.
    ///
    /// @return 0.
    virtual int send(const Iface& iface, uint16_t port, const Pkt6Ptr& pkt);

    /// @brief Simulate joining IPv6 multicast group on a socket.
    ///
    /// @note All parameters are ignored.
    ///
    /// @param sock A socket descriptor (socket must be bound).
    /// @param ifname An interface name (for link-scoped multicast groups).
    /// @param mcast A multicast address to join (e.g. "ff02::1:2").
    ///
    /// @return true if multicast join was successful
    static bool joinMulticast(int sock, const std::string& ifname,
                              const std::string & mcast);
};

} // namespace bundy::dhcp::test
} // namespace bundy::dhcp
} // namespace bundy

#endif // PKT_FILTER6_TEST_STUB_H
