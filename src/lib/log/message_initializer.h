// Copyright (C) 2011  Internet Systems Consortium, Inc. ("ISC")
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

#ifndef __MESSAGEINITIALIZER_H
#define __MESSAGEINITIALIZER_H

#include <string>
#include <vector>
#include <log/message_dictionary.h>

namespace isc {
namespace log {

/// \brief Initialize Message Dictionary
///
/// This is a helper class to add a set of message IDs and associated text to
/// the global dictionary.
///
/// It should be declared outside an execution unit and initialized with a
/// an array of values, alternating identifier, associated text and ending with
/// a NULL, e.g.
///
///     static const char* values[] = {
///         "IDENT1", "message for ident 1",
///         "IDENT2", "message for ident 2",
///             :
///         NULL
///     };
///     MessageDictionaryHelper xyz(values);
///
/// This will automatically add the message ID/text pairs to the global
/// dictionary during initialization - all that is required is that the module
/// containing the definition is included into the final executable.
///
/// Messages are added via the MessageDictionary::add() method, so any
/// duplicates are stored in the the global dictionary's overflow vector whence
/// they can be retrieved at run-time.

class MessageInitializer {
public:

    /// \brief Constructor
    ///
    /// Adds the array of values to the global dictionary, and notes any
    /// duplicates.
    ///
    /// \param values NULL-terminated array of alternating identifier strings
    /// and associated message text.
    MessageInitializer(const char* values[]);

    /// \brief Return Duplicates
    ///
    /// When messages are added to the global dictionary, any duplicates are
    /// recorded.  They can later be output through the logging system.
    ///
    /// \return List of duplicate message IDs when the global dictionary was
    /// loaded.  Note that the duplicates list itself may contain duplicates.
    static std::vector<std::string>& getDuplicates();
};

} // namespace log
} // namespace isc

#endif // __MESSAGEINITIALIZER_H