/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_stored_tracking_save.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Writes the archive test_stored_tracking_load reads.  This program never
// serializes an A through a pointer, so A, which is tracked selectively, is
// not tracked here: the archive says so, and carries no object ids for it.

#include <fstream>
#include <string>

#include <boost/archive/tmpdir.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "test_tools.hpp"

#include <boost/serialization/nvp.hpp>

struct A {
    int i;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */){
        ar & BOOST_SERIALIZATION_NVP(i);
    }
};

int test_main(int /* argc */, char * /* argv */ []){
    std::string filename = boost::archive::tmpdir();
    filename += "/stored_tracking_";
    filename += BOOST_PP_STRINGIZE(BOOST_ARCHIVE_TEST);

    const A first = { 1 };
    const A second = { 2 };
    const int last = 3;
    test_ostream os(filename.c_str(), TEST_STREAM_FLAGS);
    test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
    oa << BOOST_SERIALIZATION_NVP(first);
    oa << BOOST_SERIALIZATION_NVP(second);
    oa << BOOST_SERIALIZATION_NVP(last);
    return EXIT_SUCCESS;
}
