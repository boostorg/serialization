/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_enum.cpp

// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// Tests serialization of enumerators.  In particular it checks that a value of
// a scoped enum whose underlying type is wider than int round trips at full
// width, rather than being truncated to int on save.

#include <cstddef>
#include <cstdio>
#include <fstream>

#include <boost/config.hpp>

#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::remove;
}
#endif

#include <boost/serialization/nvp.hpp>
#include "test_tools.hpp"

// underlying type is int, so the archived form is unchanged from earlier
// releases of the library
enum color { red, green = 3, blue = 100 };

#ifndef BOOST_NO_CXX11_SCOPED_ENUMS

// the value 2^32 does not fit in a 32 bit int; a plain cast to int on save
// used to truncate it
enum class wide : long long {
    one = 1,
    big = 1LL << 32
};

// a narrow underlying type: still stored through int, so unchanged on the
// wire, but it must round trip
enum class narrow : signed char {
    minus_one = -1,
    two = 2,
    hundred = 100
};

#endif // BOOST_NO_CXX11_SCOPED_ENUMS

int test_main(int /* argc */, char * /* argv */[]){
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    const color c_save = blue;
    #ifndef BOOST_NO_CXX11_SCOPED_ENUMS
    const wide w_save = wide::big;
    const narrow n_save = narrow::minus_one;
    #endif
    {
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << boost::serialization::make_nvp("c", c_save);
        #ifndef BOOST_NO_CXX11_SCOPED_ENUMS
        oa << boost::serialization::make_nvp("w", w_save);
        oa << boost::serialization::make_nvp("n", n_save);
        #endif
    }
    color c_load = red;
    #ifndef BOOST_NO_CXX11_SCOPED_ENUMS
    wide w_load = wide::one;
    narrow n_load = narrow::two;
    #endif
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> boost::serialization::make_nvp("c", c_load);
        #ifndef BOOST_NO_CXX11_SCOPED_ENUMS
        ia >> boost::serialization::make_nvp("w", w_load);
        ia >> boost::serialization::make_nvp("n", n_load);
        #endif
    }
    BOOST_CHECK(c_save == c_load);
    #ifndef BOOST_NO_CXX11_SCOPED_ENUMS
    BOOST_CHECK(w_save == w_load);
    // the point of the test: the value survived without being truncated
    BOOST_CHECK(static_cast<long long>(w_load) == (1LL << 32));
    BOOST_CHECK(n_save == n_load);
    #endif

    std::remove(testfile);
    return EXIT_SUCCESS;
}
