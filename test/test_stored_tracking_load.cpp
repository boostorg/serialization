/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_stored_tracking_load.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Reads the archive test_stored_tracking_save writes.  Unlike that program,
// this one serializes an A through a pointer, so here A is tracked.  The
// archive must still be read as it was written, following the tracking level
// stored in it rather than the one this program would use.

// Raised by Robert Ramey in
// https://github.com/boostorg/serialization/issues/183.  Thanks!

#include <fstream>
#include <memory>
#include <string>

#include <boost/archive/tmpdir.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "test_tools.hpp"

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/unique_ptr.hpp>

struct A {
    int i;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */){
        ar & BOOST_SERIALIZATION_NVP(i);
    }
};

// Never called.  Compiling it is enough: it gives this program a pointer
// serializer for A, which exists before main() runs.
void load_through_pointer(test_iarchive & ia){
    std::unique_ptr<A> p;
    ia >> BOOST_SERIALIZATION_NVP(p);
}

int test_main(int /* argc */, char * /* argv */ []){
    std::string filename = boost::archive::tmpdir();
    filename += "/stored_tracking_";
    filename += BOOST_PP_STRINGIZE(BOOST_ARCHIVE_TEST);

    A first = { 0 };
    A second = { 0 };
    int last = 0;
    test_istream is(filename.c_str(), TEST_STREAM_FLAGS);
    test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
    ia >> BOOST_SERIALIZATION_NVP(first);
    ia >> BOOST_SERIALIZATION_NVP(second);
    ia >> BOOST_SERIALIZATION_NVP(last);
    BOOST_CHECK(1 == first.i);
    BOOST_CHECK(2 == second.i);
    BOOST_CHECK(3 == last);
    return EXIT_SUCCESS;
}
