/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_xml_destroy_after_caught_error.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// Asking an XML input archive for a name which is not there throws, and the
// caller is entitled to catch that and carry on.  The failed read has by
// then consumed the closing document tag, so the destructor finds end of
// input where it expected the trailer.  That is normal termination, not a
// stream error, and the (implicitly noexcept) destructor has to complete
// cleanly rather than terminate the process.

// This is the route into windup() that issue #99 named but left untested:
// test_xml_trailing_whitespace covers a truncated archive, where the tag was
// never written, while here the archive is well formed and an earlier failed
// read ate the tag.  The archive is destroyed normally, not while an
// exception is in flight.

// Reported by tsondergaard in
// https://github.com/boostorg/serialization/issues/109, with a program this
// test follows closely.  Thanks!

#include <sstream>
#include <string>

#include <boost/archive/archive_exception.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>

#include "test_tools.hpp"

int test_main(int /* argc */, char * /* argv */ []){
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

    int y = 0;
    bool threw = false;
    {
        std::istringstream is(content);
        boost::archive::xml_iarchive ia(is);
        ia >> boost::serialization::make_nvp("x", y);

        BOOST_TRY {
            int z = 0;
            ia >> boost::serialization::make_nvp("not_there", z);
        }
        BOOST_CATCH(const boost::archive::archive_exception &){
            threw = true;
        }
        BOOST_CATCH_END

        // `ia` is destroyed here, with the closing tag already eaten by the
        // read which failed, and with no exception in flight.
    }

    BOOST_CHECK(threw);
    BOOST_CHECK(42 == y);

    return EXIT_SUCCESS;
}
