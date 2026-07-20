/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_xml_missing_nvp.cpp

// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// Regression test for issue #109.  Attempting to read an nvp that is not
// present throws archive_exception, which the caller may catch.  The failed
// read consumes the closing tag, so at destruction windup() reaches end of
// input -- the same Boost 1.66 my_parse regression as #82/#99, reached by a
// different path.  The input archive must still destruct without terminating.

#include <sstream>
#include <string>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/archive_exception.hpp>
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

    int x = 0;
    bool caught = false;
    {
        std::istringstream is(content);
        boost::archive::xml_iarchive ia(is);
        ia >> boost::serialization::make_nvp("x", x);
        try {
            int y = 0;
            ia >> boost::serialization::make_nvp("not_there", y);
        } catch (const boost::archive::archive_exception &){
            caught = true;
        }
        // `ia` is destroyed here, after the caught exception.  Before the fix,
        // windup() hit end of input (the failed read consumed the closing
        // tag) and threw out of the noexcept destructor, terminating.
    }
    BOOST_CHECK(42 == x);
    BOOST_CHECK(caught);

    return EXIT_SUCCESS;
}
