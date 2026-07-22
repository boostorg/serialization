/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_class_template_version.cpp

// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// Tests BOOST_CLASS_TEMPLATE_VERSION, which assigns a serialization version
// to a class template through a partial specialization of the version trait.

#include <cstddef>
#include <cstdio>
#include <fstream>

#include <boost/config.hpp>

#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::remove;
}
#endif

#include <boost/static_assert.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
#include "test_tools.hpp"

// a class template with two type parameters; serialize records the version
// it is given on load so the round trip can be checked
template<class T, class U>
struct pair_holder {
    T first;
    U second;
    unsigned int loaded_version;
    pair_holder() : first(), second(), loaded_version(0) {}
    pair_holder(T f, U s) : first(f), second(s), loaded_version(0) {}
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version){
        loaded_version = version;
        ar & boost::serialization::make_nvp("first", first);
        ar & boost::serialization::make_nvp("second", second);
    }
};

BOOST_CLASS_TEMPLATE_VERSION((class T, class U), (pair_holder<T, U>), 5)

// a class template mixing a type parameter and a non-type parameter
template<class T, std::size_t N>
struct sized_holder {
    T value;
};

BOOST_CLASS_TEMPLATE_VERSION((class T, std::size_t N), (sized_holder<T, N>), 3)

// a template we do not version, to confirm the default is still 0
template<class T>
struct unversioned {
    T value;
};

int test_main(int /* argc */, char * /* argv */[]){
    // the macro must set the compile time version for every instantiation of
    // the template, regardless of the actual arguments
    BOOST_STATIC_ASSERT(
        (boost::serialization::version<pair_holder<int, double> >::value == 5)
    );
    BOOST_STATIC_ASSERT(
        (boost::serialization::version<pair_holder<char, long> >::value == 5)
    );
    // works for a non-type parameter too
    BOOST_STATIC_ASSERT(
        (boost::serialization::version<sized_holder<int, 4> >::value == 3)
    );
    // an unversioned template still defaults to 0
    BOOST_STATIC_ASSERT(
        (boost::serialization::version<unversioned<int> >::value == 0)
    );

    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    const pair_holder<int, double> saved(7, 2.5);
    {
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << boost::serialization::make_nvp("ph", saved);
    }
    pair_holder<int, double> loaded;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> boost::serialization::make_nvp("ph", loaded);
    }
    BOOST_CHECK(loaded.first == saved.first);
    BOOST_CHECK(loaded.second == saved.second);
    // the version set by the macro must be delivered to serialize on load
    BOOST_CHECK(loaded.loaded_version == 5);

    std::remove(testfile);
    return EXIT_SUCCESS;
}
