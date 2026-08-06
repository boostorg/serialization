/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_version_value_type.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// A version trait written by hand has to work whichever integer type it
// names for its value.  The library declares the trait as an int, but
// nothing stops a user from writing unsigned int, and doing so used to
// leave the value with no definition to link against: it was passed to
// version_type through a reference, which made a definition necessary.

// Reported by LowLevelMahn in
// https://github.com/boostorg/serialization/issues/311, together with a
// self contained example which linked with one integer type and not with
// the other.  Thanks!

// Note that this only ever failed on compilers which do not fold the
// constant away, so it links either way on some of them.

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
#include <boost/serialization/version.hpp>

#include "test_tools.hpp"

// the value spelled the way the library spells it
struct signed_version {
    int value;
};

// and spelled the way the report did, which is what used to fail
template<typename T, std::size_t N>
struct unsigned_version {
    int value;
};

namespace boost {
namespace serialization {

template<>
struct version<signed_version>
{
    BOOST_STATIC_CONSTANT(int, value = 1);
};

// a partial specialization, as in the report
template<typename T, std::size_t N>
struct version<unsigned_version<T, N> >
{
    BOOST_STATIC_CONSTANT(unsigned int, value = 2);
};

template<class Archive>
void serialize(Archive & ar, signed_version & t, const unsigned int file_version){
    BOOST_CHECK(1 == file_version);
    ar & BOOST_SERIALIZATION_NVP(t.value);
}

template<class Archive, typename T, std::size_t N>
void serialize(
    Archive & ar,
    unsigned_version<T, N> & t,
    const unsigned int file_version
){
    BOOST_CHECK(2 == file_version);
    ar & BOOST_SERIALIZATION_NVP(t.value);
}

} // namespace serialization
} // namespace boost

typedef unsigned_version<int, 3> unsigned_version_type;

int
test_main(int /* argc */, char * /* argv */ [])
{
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    {
        signed_version a;
        a.value = 11;
        unsigned_version_type b;
        b.value = 22;
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << BOOST_SERIALIZATION_NVP(a);
        oa << BOOST_SERIALIZATION_NVP(b);
    }
    {
        signed_version a;
        a.value = 0;
        unsigned_version_type b;
        b.value = 0;
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> BOOST_SERIALIZATION_NVP(a);
        ia >> BOOST_SERIALIZATION_NVP(b);
        BOOST_CHECK(11 == a.value);
        BOOST_CHECK(22 == b.value);
    }

    std::remove(testfile);
    return EXIT_SUCCESS;
}
