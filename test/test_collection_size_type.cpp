/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_collection_size_type.cpp

// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// Exercises the interface of collection_size_type.  See also
// issue #192.

#include <cstddef>
#include <cstdio>
#include <fstream>

#include <boost/config.hpp>

#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::remove;
}
#endif

#include <boost/serialization/collection_size_type.hpp>
#include <boost/serialization/nvp.hpp>
#include "test_tools.hpp"

int test_main(int /* argc */, char * /* argv */[]){
    using boost::serialization::collection_size_type;

    collection_size_type c(5);
    BOOST_CHECK(static_cast<std::size_t>(c) == 5);
    BOOST_CHECK(c == collection_size_type(5));
    BOOST_CHECK(collection_size_type(4) < c);

    ++c;
    BOOST_CHECK(static_cast<std::size_t>(c) == 6);
    --c;
    BOOST_CHECK(static_cast<std::size_t>(c) == 5);

    const collection_size_type post_inc = c++;
    BOOST_CHECK(static_cast<std::size_t>(post_inc) == 5);
    BOOST_CHECK(static_cast<std::size_t>(c) == 6);
    const collection_size_type post_dec = c--;
    BOOST_CHECK(static_cast<std::size_t>(post_dec) == 6);
    BOOST_CHECK(static_cast<std::size_t>(c) == 5);

    collection_size_type count(3);
    std::size_t iterations = 0;
    while(count-- > 0){
        ++iterations;
    }
    BOOST_CHECK(iterations == 3);

    // round trip through an archive
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);
    const collection_size_type saved(1234);
    {
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << boost::serialization::make_nvp("count", saved);
    }
    collection_size_type loaded(0);
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> boost::serialization::make_nvp("count", loaded);
    }
    BOOST_CHECK(static_cast<std::size_t>(loaded) == 1234);

    std::remove(testfile);
    return EXIT_SUCCESS;
}
