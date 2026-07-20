/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_xml_trailing_whitespace.cpp

// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// Regression test for issue #99.  When an XML input archive is destroyed and
// its stream has no closing tag left -- only trailing whitespace before end
// of input (a truncated archive; the reported case had the stream "contain
// \r\n") -- windup() reaches end of input while scanning for the trailing
// tag.  End of input there is normal termination, not a stream error, so the
// (implicitly noexcept) destructor must complete cleanly rather than throw,
// which used to terminate the process.

#include <sstream>
#include <string>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/nvp.hpp>

#include "test_tools.hpp"

int test_main(int /* argc */, char * /* argv */ []){
    // Build a valid XML archive holding a single value.
    std::string content;
    {
        std::ostringstream os;
        {
            boost::archive::xml_oarchive oa(os);
            const int x = 42;
            oa << boost::serialization::make_nvp("x", x);
        }
        content = os.str();
    }

    // Drop the closing document tag and leave only trailing whitespace, so the
    // input archive's windup() reaches end of input at destruction instead of
    // finding a tag.
    const std::string::size_type close = content.rfind("</boost_serialization>");
    BOOST_REQUIRE(std::string::npos != close);
    content.erase(close);
    content += "\r\n";

    int y = 0;
    {
        std::istringstream is(content);
        boost::archive::xml_iarchive ia(is);
        ia >> boost::serialization::make_nvp("x", y);
        // `ia` is destroyed here with only trailing whitespace left.  Before
        // the fix, windup() threw input_stream_error out of the noexcept
        // destructor and terminated the process.
    }
    BOOST_CHECK(42 == y);

    return EXIT_SUCCESS;
}
