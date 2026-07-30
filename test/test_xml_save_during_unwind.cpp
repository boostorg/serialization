/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_xml_save_during_unwind.cpp

// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// Regression test for issue #188.  The XML output archive writes the closing
// </boost_serialization> tag in its destructor.  It used to skip that write
// whenever an exception was unwinding the stack (uncaught_exceptions() > 0),
// which silently corrupted an otherwise complete archive when it happened to
// be destroyed during unwinding for an unrelated reason.  A complete document
// must be closed regardless; only a genuinely interrupted one (an element still
// open) may be left unterminated.

#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>

#include "test_tools.hpp"

int test_main(int /* argc */, char * /* argv */ []){
    // Serialize a complete value, then throw an unrelated exception so the
    // archive is destroyed while the stack unwinds.  `os` is declared outside
    // the try block so it outlives the archive and we can inspect what the
    // destructor wrote.
    std::ostringstream os;
    try {
        boost::archive::xml_oarchive oa(os);
        const int x = 42;
        oa << boost::serialization::make_nvp("x", x);
        throw std::runtime_error("unrelated");
    }
    catch(const std::runtime_error &){}

    const std::string content = os.str();

    // The complete document must have been closed despite the unwinding.
    BOOST_CHECK(
        content.find("</boost_serialization>") != std::string::npos
    );

    // And it must load back cleanly.
    int y = 0;
    {
        std::istringstream is(content);
        boost::archive::xml_iarchive ia(is);
        ia >> boost::serialization::make_nvp("x", y);
    }
    BOOST_CHECK(42 == y);

    return EXIT_SUCCESS;
}
