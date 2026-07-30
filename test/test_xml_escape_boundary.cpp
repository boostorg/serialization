/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_xml_escape_boundary.cpp

// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// Regression test for issue #229.  Writing a std::string that contains a
// character needing an XML escape (here '"' -> &quot;) whose expansion
// straddles the 32 character internal buffer of wchar_from_mb used to
// produce malformed output such as "&qu&quot;": the xml_escape iterator was
// copied in mid escape sequence and its hand-written copy constructor dropped
// the escape<> base's state, restarting the sequence.  Check that a wide XML
// archive escapes such a string correctly and round-trips it.

#include <sstream>
#include <string>

#include <boost/archive/xml_woarchive.hpp>
#include <boost/archive/xml_wiarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

#include "test_tools.hpp"

int test_main(int /* argc */, char * /* argv */ []){
    // The '"' sits at index 29, so its &quot; expansion crosses the 32 char
    // wchar_from_mb buffer boundary.
    const std::string instring("01234567890123456789012345678\"-here-is-the-error");

    std::wstring archived;
    {
        std::wostringstream os;
        {
            boost::archive::xml_woarchive oa(os);
            oa << boost::serialization::make_nvp("err", instring);
        }
        archived = os.str();
    }

    // The escape must appear exactly once and intact, never as the truncated
    // "&qu&quot;" the bug produced.
    BOOST_CHECK(archived.find(L"&qu&quot;") == std::wstring::npos);
    BOOST_CHECK(archived.find(L"&quot;") != std::wstring::npos);

    // And it must round-trip.
    std::string result;
    {
        std::wistringstream is(archived);
        boost::archive::xml_wiarchive ia(is);
        ia >> boost::serialization::make_nvp("err", result);
    }
    BOOST_CHECK(instring == result);

    return EXIT_SUCCESS;
}
