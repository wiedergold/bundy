// Copyright (C) 2010  Internet Systems Consortium, Inc. ("ISC")
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

// $Id$

#include <recurse/recursor.h>
#include <testutils/srv_unittest.h>

namespace {
class RecursorConfig : public ::testing::Test {
    public:
        IOService ios;
        DNSService dnss;
        Recursor server;
        RecursorConfig() :
            dnss(ios, NULL, NULL, NULL)
        {
            server.setDNSService(dnss);
        }
        void invalidTest(const string &JOSN);
};

TEST_F(RecursorConfig, forwardAddresses) {
    // Default value should be fully recursive
    EXPECT_TRUE(server.getForwardAddresses().empty());
    EXPECT_FALSE(server.isForwarding());

    // Try putting there some addresses
    vector<pair<string, uint16_t> > addresses;
    addresses.push_back(pair<string, uint16_t>(DEFAULT_REMOTE_ADDRESS, 53));
    addresses.push_back(pair<string, uint16_t>("::1", 53));
    server.setForwardAddresses(addresses);
    EXPECT_EQ(2, server.getForwardAddresses().size());
    EXPECT_EQ("::1", server.getForwardAddresses()[1].first);
    EXPECT_TRUE(server.isForwarding());

    // Is it independent from what we do with the vector later?
    addresses.clear();
    EXPECT_EQ(2, server.getForwardAddresses().size());

    // Did it return to fully recursive?
    server.setForwardAddresses(addresses);
    EXPECT_TRUE(server.getForwardAddresses().empty());
    EXPECT_FALSE(server.isForwarding());
}

TEST_F(RecursorConfig, forwardAddressConfig) {
    // Try putting there some address
    ElementPtr config(Element::fromJSON("{"
        "\"forward_addresses\": ["
        "   {"
        "       \"address\": \"192.0.2.1\","
        "       \"port\": 53"
        "   }"
        "]"
        "}"));
    ConstElementPtr result(server.updateConfig(config));
    EXPECT_EQ(result->toWire(), isc::config::createAnswer()->toWire());
    EXPECT_TRUE(server.isForwarding());
    ASSERT_EQ(1, server.getForwardAddresses().size());
    EXPECT_EQ("192.0.2.1", server.getForwardAddresses()[0].first);
    EXPECT_EQ(53, server.getForwardAddresses()[0].second);

    // And then remove all addresses
    config = Element::fromJSON("{"
        "\"forward_addresses\": null"
        "}");
    result = server.updateConfig(config);
    EXPECT_EQ(result->toWire(), isc::config::createAnswer()->toWire());
    EXPECT_FALSE(server.isForwarding());
    EXPECT_EQ(0, server.getForwardAddresses().size());
}

void
RecursorConfig::invalidTest(const string &JOSN) {
    ElementPtr config(Element::fromJSON(JOSN));
    EXPECT_FALSE(server.updateConfig(config)->equals(
        *isc::config::createAnswer())) << "Accepted config " << JOSN << endl;
}

TEST_F(RecursorConfig, invalidForwardAddresses) {
    // Try torturing it with some invalid inputs
    invalidTest("{"
        "\"forward_addresses\": \"error\""
        "}");
    invalidTest("{"
        "\"forward_addresses\": [{}]"
        "}");
    invalidTest("{"
        "\"forward_addresses\": [{"
        "   \"port\": 1.5,"
        "   \"address\": \"192.0.2.1\""
        "}]}");
    invalidTest("{"
        "\"forward_addresses\": [{"
        "   \"port\": -5,"
        "   \"address\": \"192.0.2.1\""
        "}]}");
    invalidTest("{"
        "\"forward_addresses\": [{"
        "   \"port\": 53,"
        "   \"address\": \"bad_address\""
        "}]}");
}

TEST_F(RecursorConfig, listenAddresses) {
    // Default value should be fully recursive
    EXPECT_TRUE(server.getListenAddresses().empty());

    // Try putting there some addresses
    vector<pair<string, uint16_t> > addresses;
    addresses.push_back(pair<string, uint16_t>("127.0.0.1", 5300));
    addresses.push_back(pair<string, uint16_t>("::1", 5300));
    server.setListenAddresses(addresses);
    EXPECT_EQ(2, server.getListenAddresses().size());
    EXPECT_EQ("::1", server.getListenAddresses()[1].first);

    // Is it independent from what we do with the vector later?
    addresses.clear();
    EXPECT_EQ(2, server.getListenAddresses().size());

    // Did it return to fully recursive?
    server.setListenAddresses(addresses);
    EXPECT_TRUE(server.getListenAddresses().empty());
}

TEST_F(RecursorConfig, DISABLED_listenAddressConfig) {
    // Try putting there some address
    ElementPtr config(Element::fromJSON("{"
        "\"listen_on\": ["
        "   {"
        "       \"address\": \"127.0.0.1\","
        "       \"port\": 5300"
        "   }"
        "]"
        "}"));
    ConstElementPtr result(server.updateConfig(config));
    EXPECT_EQ(result->toWire(), isc::config::createAnswer()->toWire());
    ASSERT_EQ(1, server.getListenAddresses().size());
    EXPECT_EQ("127.0.0.1", server.getListenAddresses()[0].first);
    EXPECT_EQ(5300, server.getListenAddresses()[0].second);

    // As this is example address, the machine should not have it on
    // any interface
    // FIXME: This test aborts, because it tries to rollback and
    //     it is impossible, since the sockets are not closed.
    //     Once #388 is solved, enable this test.
    config = Element::fromJSON("{"
        "\"listen_on\": ["
        "   {"
        "       \"address\": \"192.0.2.0\","
        "       \"port\": 5300"
        "   }"
        "]"
        "}");
    result = server.updateConfig(config);
    EXPECT_FALSE(result->equals(*isc::config::createAnswer()));
    ASSERT_EQ(1, server.getListenAddresses().size());
    EXPECT_EQ("127.0.0.1", server.getListenAddresses()[0].first);
    EXPECT_EQ(5300, server.getListenAddresses()[0].second);
}

TEST_F(RecursorConfig, invalidListenAddresses) {
    // Try torturing it with some invalid inputs
    invalidTest("{"
        "\"listen_on\": \"error\""
        "}");
    invalidTest("{"
        "\"listen_on\": [{}]"
        "}");
    invalidTest("{"
        "\"listen_on\": [{"
        "   \"port\": 1.5,"
        "   \"address\": \"192.0.2.1\""
        "}]}");
    invalidTest("{"
        "\"listen_on\": [{"
        "   \"port\": -5,"
        "   \"address\": \"192.0.2.1\""
        "}]}");
    invalidTest("{"
        "\"listen_on\": [{"
        "   \"port\": 53,"
        "   \"address\": \"bad_address\""
        "}]}");
}

// Just test it sets and gets the values correctly
TEST_F(RecursorConfig, timeouts) {
    server.setTimeouts(0, 1);
    EXPECT_EQ(0, server.getTimeouts().first);
    EXPECT_EQ(1, server.getTimeouts().second);
    server.setTimeouts();
    EXPECT_EQ(-1, server.getTimeouts().first);
    EXPECT_EQ(0, server.getTimeouts().second);
}

TEST_F(RecursorConfig, timeoutsConfig) {
    ElementPtr config = Element::fromJSON("{"
            "\"timeout\": 1000,"
            "\"retries\": 3"
            "}");
    ConstElementPtr result(server.updateConfig(config));
    EXPECT_EQ(result->toWire(), isc::config::createAnswer()->toWire());
    EXPECT_EQ(1000, server.getTimeouts().first);
    EXPECT_EQ(3, server.getTimeouts().second);
}

TEST_F(RecursorConfig, invalidTimeoutsConfig) {
    invalidTest("{"
        "\"timeout\": \"error\""
        "}");
    invalidTest("{"
        "\"timeout\": -2"
        "}");
    invalidTest("{"
        "\"retries\": \"error\""
        "}");
    invalidTest("{"
        "\"retries\": -1"
        "}");
}

}