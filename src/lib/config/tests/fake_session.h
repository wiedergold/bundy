// Copyright (C) 2009  Internet Systems Consortium, Inc. ("ISC")
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

#ifndef BUNDY_FAKESESSION_H
#define BUNDY_FAKESESSION_H 1

#include <string>

#include <boost/function.hpp>

#include <exceptions/exceptions.h>

#include <cc/data.h>
#include <cc/session.h>

namespace bundy {
namespace cc {
class FakeSession : public AbstractSession {
private:
    FakeSession(const Session& source);
    FakeSession& operator=(const Session& source);

public:
    // if initial_messages contains a list of messages,
    // these are sent when recv_msg or group_recvmsg is called
    // instead of whatever is in the msg queue.
    // The test can also add data to a copy of the message later to tweak
    // the group_recvmsg() behavior.  See getMessages() below.
    FakeSession(bundy::data::ElementPtr initial_messages,
                bundy::data::ElementPtr subscriptions,
                bundy::data::ElementPtr msg_queue);
    virtual ~FakeSession();

    // This is thrown if two reads for length at once are scheduled at once.
    // Such thing does bad things currently (see discussion in ticket #931).
    class DoubleRead : public Exception {
    public:
        DoubleRead(const char* file, size_t line, const char* what) :
            Exception(file, line, what) {}
    };

    virtual void startRead(boost::function<void()> read_callback);

    virtual void establish(const char* socket_file = NULL);
    virtual void disconnect();
    virtual void subscribe(std::string group,
                           std::string instance = "*");
    virtual void unsubscribe(std::string group,
                             std::string instance = "*");
    virtual int group_sendmsg(bundy::data::ConstElementPtr msg,
                              std::string group,
                              std::string instance = "*",
                              std::string to = "*",
                              bool want_answer = false);
    virtual bool group_recvmsg(bundy::data::ConstElementPtr& envelope,
                               bundy::data::ConstElementPtr& msg,
                               bool nonblock = true,
                               int seq = -1);
    virtual int reply(bundy::data::ConstElementPtr envelope,
                      bundy::data::ConstElementPtr newmsg);
    virtual bool hasQueuedMsgs() const;
    virtual void setTimeout(size_t) {}
    virtual size_t getTimeout() const { return (0); }
    bundy::data::ConstElementPtr getFirstMessage(std::string& group,
                                               std::string& to) const;
    void addMessage(bundy::data::ConstElementPtr, const std::string& group,
                    const std::string& to, int seq = -1,
                    bool want_answer = false);
    bool haveSubscription(const std::string& group,
                          const std::string& instance);
    bool haveSubscription(const bundy::data::ConstElementPtr group,
                          const bundy::data::ConstElementPtr instance);

    // For the convenience of tests, we share these internal members
    // with the tester.  The test code may insert update and check,
    // before (via the constructor parameters), during and after the actual
    // session object was created/destroyed.
    bundy::data::ElementPtr getMessages() { return (messages_); }
    bundy::data::ElementPtr getMsgQueue() { return (msg_queue_); }

    /// Throw exception on sendmsg()
    ///
    /// When set to true, and sendmsg() is later called, this
    /// will throw bundy::Exception
    ///
    /// \param value If true, enable throw. If false, disable it
    void setThrowOnSend(bool value) { throw_on_send_ = value; }

private:
    bool recvmsg(bundy::data::ConstElementPtr& msg,
                 bool nonblock = true, int seq = -1);
    bool recvmsg(bundy::data::ConstElementPtr& env,
                 bundy::data::ConstElementPtr& msg,
                 bool nonblock = true, int seq = -1);

    const bundy::data::ElementPtr messages_;
    bundy::data::ElementPtr subscriptions_;
    bundy::data::ElementPtr msg_queue_;
    bool started_;
    bool throw_on_send_;
};
} // namespace cc
} // namespace bundy

#endif // BUNDY_FAKESESSION_H

// Local Variables:
// mode: c++
// End:
