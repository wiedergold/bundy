// Copyright (C) 2012  Internet Systems Consortium, Inc. ("ISC")
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

#include <dns/rrset_collection.h>
#include <dns/rrttl.h>
#include <dns/rdataclass.h>

#include <gtest/gtest.h>

using namespace isc::dns;
using namespace isc::dns::rdata;
using namespace std;

namespace {

class RRsetCollectionTest : public ::testing::Test {
public:
    RRsetCollectionTest() :
        rrclass("IN"),
        origin("example.org"),
        collection(TEST_DATA_SRCDIR "/example.org", origin, rrclass)
    {}

    const RRClass rrclass;
    const Name origin;
    RRsetCollection collection;
};

TEST_F(RRsetCollectionTest, findBase) {
    // Test the find() that returns isc::dns::AbstractRRset*
    const AbstractRRset* rrset = collection.find(Name("www.example.org"),
                                                 RRType::A());
    EXPECT_NE(static_cast<AbstractRRset*>(NULL), rrset);
    EXPECT_EQ(RRType::A(), rrset->getType());
    EXPECT_EQ(RRTTL(3600), rrset->getTTL());
    EXPECT_EQ(RRClass("IN"), rrset->getClass());
    EXPECT_EQ(Name("www.example.org"), rrset->getName());

    // foo.example.org doesn't exist
    rrset = collection.find(Name("foo.example.org"), RRType::A());
    EXPECT_EQ(static_cast<AbstractRRset*>(NULL), rrset);

    // www.example.org exists, but not with MX
    rrset = collection.find(Name("www.example.org"), RRType::MX());
    EXPECT_EQ(static_cast<AbstractRRset*>(NULL), rrset);

    // www.example.org exists, with AAAA
    rrset = collection.find(Name("www.example.org"), RRType::AAAA());
    EXPECT_NE(static_cast<AbstractRRset*>(NULL), rrset);
}

template <typename T, typename TP>
void doFind(T& collection, const RRClass& rrclass) {
    // Test the find() that returns ConstRRsetPtr
    TP rrset = collection.find(Name("www.example.org"), rrclass, RRType::A());
    EXPECT_TRUE(rrset);
    EXPECT_EQ(RRType::A(), rrset->getType());
    EXPECT_EQ(RRTTL(3600), rrset->getTTL());
    EXPECT_EQ(RRClass("IN"), rrset->getClass());
    EXPECT_EQ(Name("www.example.org"), rrset->getName());

    // foo.example.org doesn't exist
    rrset = collection.find(Name("foo.example.org"), rrclass, RRType::A());
    EXPECT_FALSE(rrset);

    // www.example.org exists, but not with MX
    rrset = collection.find(Name("www.example.org"), rrclass, RRType::MX());
    EXPECT_FALSE(rrset);

    // www.example.org exists, with AAAA
    rrset = collection.find(Name("www.example.org"), rrclass, RRType::AAAA());
    EXPECT_TRUE(rrset);
}

TEST_F(RRsetCollectionTest, findConst) {
    // Test the find() that returns ConstRRsetPtr
    const RRsetCollection& ccln = collection;
    doFind<const RRsetCollection, ConstRRsetPtr>(ccln, rrclass);
}

TEST_F(RRsetCollectionTest, find) {
    // Test the find() that returns RRsetPtr
    doFind<RRsetCollection, RRsetPtr>(collection, rrclass);
}

TEST_F(RRsetCollectionTest, addAndRemove) {
    // foo.example.org/A doesn't exist
    RRsetPtr rrset_found = collection.find(Name("foo.example.org"), rrclass,
                                           RRType::A());
    EXPECT_FALSE(rrset_found);

    // Add foo.example.org/A
    RRsetPtr rrset(new BasicRRset(Name("foo.example.org"), rrclass, RRType::A(),
                                  RRTTL(7200)));
    rrset->addRdata(in::A("192.0.2.1"));
    collection.addRRset(rrset);

    // foo.example.org/A should now exist
    rrset_found = collection.find(Name("foo.example.org"), rrclass,
                                  RRType::A());
    EXPECT_TRUE(rrset_found);
    EXPECT_EQ(RRType::A(), rrset_found->getType());
    EXPECT_EQ(RRTTL(7200), rrset_found->getTTL());
    EXPECT_EQ(RRClass("IN"), rrset_found->getClass());
    EXPECT_EQ(Name("foo.example.org"), rrset_found->getName());

    // Remove foo.example.org/A
    collection.removeRRset(Name("foo.example.org"), rrclass, RRType::A());

    // foo.example.org/A should not exist now
    rrset_found = collection.find(Name("foo.example.org"), rrclass,
                                  RRType::A());
    EXPECT_FALSE(rrset_found);
}

TEST_F(RRsetCollectionTest, iteratorTest) {
    RRsetCollection::iterator iter = collection.begin();
    EXPECT_TRUE(iter != collection.end());

    // Here, we just count the records and do some basic tests on them.
    size_t count = 0;
    for (RRsetCollection::iterator it = collection.begin();
         it != collection.end(); ++it) {
         ++count;
         const AbstractRRset& rrset = *it;
         EXPECT_EQ(rrclass, rrset.getClass());
         EXPECT_EQ(RRTTL(3600), rrset.getTTL());
    }

    // example.org master file has SOA, NS, A, AAAA
    EXPECT_EQ(4, count);
}

} // namespace