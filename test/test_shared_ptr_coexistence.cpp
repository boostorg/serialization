/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_shared_ptr_coexistence.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// Including shared_ptr.hpp must leave the serialization of every other type
// alone.  A declaration in shared_ptr_helper.hpp used to add an overload to
// boost::serialization::load matching any class template taking one type
// argument, so that a std::optional or a std::vector serialized in the same
// translation unit no longer compiled.

// Reported by sowle in
// https://github.com/boostorg/serialization/issues/319, with a reduced
// example that took considerable effort to arrive at.  correaa pinned the
// failure down to the exact declaration, and olologin contributed the
// std::vector case, which is covered here as well.  Thanks to all three.

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <vector>

#include <boost/config.hpp>
#ifndef BOOST_NO_CXX17_HDR_OPTIONAL
#include <optional>
#endif

#include <boost/make_shared.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/vector.hpp>

#include "test_tools.hpp"

// Included last on purpose: the original report reached the failure with
// this header sitting after everything else.
#include <boost/serialization/shared_ptr.hpp>

struct payload {
    int m_x;
    payload() : m_x(0) {}
    explicit payload(int x) : m_x(x) {}
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */){
        ar & boost::serialization::make_nvp("x", m_x);
    }
    bool operator==(const payload & rhs) const {
        return m_x == rhs.m_x;
    }
};

int
test_main(int /* argc */, char * /* argv */ [])
{
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    const std::vector<int> v(3, 7);
    const boost::optional<payload> bo(payload(11));
    const boost::shared_ptr<payload> sp = boost::make_shared<payload>(13);
    #ifndef BOOST_NO_CXX17_HDR_OPTIONAL
    const std::optional<payload> so(payload(17));
    #endif
    {
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << boost::serialization::make_nvp("v", v);
        oa << boost::serialization::make_nvp("bo", bo);
        oa << boost::serialization::make_nvp("sp", sp);
        #ifndef BOOST_NO_CXX17_HDR_OPTIONAL
        oa << boost::serialization::make_nvp("so", so);
        #endif
    }

    std::vector<int> v2;
    boost::optional<payload> bo2;
    boost::shared_ptr<payload> sp2;
    #ifndef BOOST_NO_CXX17_HDR_OPTIONAL
    std::optional<payload> so2;
    #endif
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> boost::serialization::make_nvp("v", v2);
        ia >> boost::serialization::make_nvp("bo", bo2);
        ia >> boost::serialization::make_nvp("sp", sp2);
        #ifndef BOOST_NO_CXX17_HDR_OPTIONAL
        ia >> boost::serialization::make_nvp("so", so2);
        #endif
    }

    BOOST_CHECK(v == v2);
    BOOST_CHECK(bo == bo2);
    BOOST_REQUIRE(NULL != sp2.get());
    BOOST_CHECK(*sp == *sp2);
    #ifndef BOOST_NO_CXX17_HDR_OPTIONAL
    BOOST_CHECK(so == so2);
    #endif

    std::remove(testfile);
    return EXIT_SUCCESS;
}
