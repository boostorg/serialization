#ifndef BOOST_ARCHIVE_DETAIL_STREAM_POSITION_MESSAGE_HPP
#define BOOST_ARCHIVE_DETAIL_STREAM_POSITION_MESSAGE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// stream_position_message.hpp: a string describing where an input stream
// error was met

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef>
#include <ios>
#include <streambuf>
#include <string>

namespace boost {
namespace archive {
namespace detail {

// The buffer is asked, rather than the stream it belongs to, because a
// stream in a fail state answers -1 whatever its position really is.  A
// buffer which is absent, or which cannot seek, answers -1 as well, and is
// reported as an unknown position.
template<class Elem, class Tr>
std::string stream_position_message(
    std::basic_streambuf<Elem, Tr> * const sb
){
    const std::streampos pos = (NULL == sb)
        ? std::streampos(-1)
        : sb->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
    return "at offset "
        + (std::streampos(-1) == pos
            ? std::string("<unknown>")
            : std::to_string(static_cast<std::streamoff>(pos)));
}

}
}
}

#endif // BOOST_ARCHIVE_DETAIL_STREAM_POSITION_MESSAGE_HPP
