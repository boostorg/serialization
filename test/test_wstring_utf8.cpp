/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_wstring_utf8.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// A std::wstring holding characters outside ASCII has to survive a round
// trip.  The XML archives are the ones which have to encode it, since xml
// is text, and both ends have to agree on UTF-8.

// Reported by schorsch1976 in
// https://github.com/boostorg/serialization/issues/298, with a reproducer
// showing that only the xml archive was affected while text and binary,
// which write the characters unchanged, were not.  Thanks!

// The characters are written as universal character names on purpose, so
// that the test does not depend on how the compiler reads this file.

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <string>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::remove;
}
#endif

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

#include "test_tools.hpp"

#ifndef BOOST_NO_STD_WSTRING

struct data
{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & BOOST_SERIALIZATION_NVP(value);
        ar & BOOST_SERIALIZATION_NVP(tail);
    }
    std::wstring value;
    int tail;
};

static void round_trip(const char * testfile, const std::wstring & w){
    {
        data d;
        d.value = w;
        d.tail = 42;
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << BOOST_SERIALIZATION_NVP(d);
    }
    data d;
    d.tail = 0;
    test_istream is(testfile, TEST_STREAM_FLAGS);
    test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
    ia >> BOOST_SERIALIZATION_NVP(d);
    BOOST_CHECK(d.value == w);
    // whatever follows the string has to be readable as well
    BOOST_CHECK(42 == d.tail);
}

int
test_main(int /* argc */, char * /* argv */ [])
{
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    // two bytes and three bytes each once encoded as UTF-8
    const std::wstring two(L"\u00E9\u00DF");
    const std::wstring three(L"\u4F60\u597D");

    round_trip(testfile, L"");
    round_trip(testfile, L"plain ascii");
    round_trip(testfile, two);
    round_trip(testfile, three);
    round_trip(testfile, L"mixed " + two + L" and " + three + L" text");

    // The decoder fills a 32 byte buffer without regard to character
    // boundaries, so walk a multibyte character across that boundary.
    for(std::size_t pad = 26; pad <= 38; ++pad){
        round_trip(testfile, std::wstring(pad, L'x') + three);
    }

    // longer than the buffer, so that it is filled repeatedly
    std::wstring big;
    for(int i = 0; i < 40; ++i){
        big += three;
        big += L"ascii";
    }
    round_trip(testfile, big);

    std::remove(testfile);
    return EXIT_SUCCESS;
}

#else // BOOST_NO_STD_WSTRING

int
test_main(int /* argc */, char * /* argv */ [])
{
    return EXIT_SUCCESS;
}

#endif // BOOST_NO_STD_WSTRING
