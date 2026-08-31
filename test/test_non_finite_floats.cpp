/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_non_finite_floats.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// An infinity is written as "inf" and a NaN as "nan", because that is what
// the stream writes, and extraction of a floating point number then refuses
// both: it accepts digits and little else, in libstdc++ and in the Microsoft
// library alike.  So the library used to write text archives which it could
// not read back.

// Reported by nim65s in
// https://github.com/boostorg/serialization/issues/386.  Thanks!

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <limits>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::remove;
}
#endif

#include "test_tools.hpp"

#include <boost/serialization/nvp.hpp>

template<class T>
struct values {
    T positive_infinity;
    T negative_infinity;
    T not_a_number;
    T ordinary;

    values() :
        positive_infinity(std::numeric_limits<T>::infinity()),
        negative_infinity(-std::numeric_limits<T>::infinity()),
        not_a_number(std::numeric_limits<T>::quiet_NaN()),
        ordinary(T(24.567))
    {}

    // Deliberately not the constructor above: this one is what the load
    // fills in, and it must start from something finite so that a load which
    // quietly does nothing cannot pass.
    values(int) :
        positive_infinity(0),
        negative_infinity(0),
        not_a_number(0),
        ordinary(0)
    {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */){
        ar & boost::serialization::make_nvp("pos_inf", positive_infinity);
        ar & boost::serialization::make_nvp("neg_inf", negative_infinity);
        ar & boost::serialization::make_nvp("nan", not_a_number);
        ar & boost::serialization::make_nvp("ordinary", ordinary);
    }
};

template<class T>
void test_type(){
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    const values<T> written;
    {
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << boost::serialization::make_nvp("values", written);
    }

    values<T> read(0);
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> boost::serialization::make_nvp("values", read);
    }

    BOOST_CHECK(read.positive_infinity == std::numeric_limits<T>::infinity());
    BOOST_CHECK(read.negative_infinity == -std::numeric_limits<T>::infinity());
    // A NaN is equal to nothing, itself included, so that is the test.
    BOOST_CHECK(read.not_a_number != read.not_a_number);
    BOOST_CHECK(read.ordinary == written.ordinary);

    std::remove(testfile);
}

int test_main(int /* argc */, char * /* argv */ []){
    test_type<float>();
    test_type<double>();
    return EXIT_SUCCESS;
}
