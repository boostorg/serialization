/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_dll_tracking.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Whether a type is tracked is settled per module: the serializer of a module
// which serializes the type through a pointer tracks it, the others don't.
// When an archive gets the same type from the serializers of two modules, the
// tracking level stored for it must still match whether object ids follow.
// For dll_tracked_base, the test's untracked serializer comes first, and the
// library's tracked one follows; for dll_untracked_base, the test's tracked
// serializer is registered first, by the save through a base pointer, and the
// library's untracked one saves the base.  The first order failed up to Boost
// 1.83, the second one from 1.84 to 1.92.

// Reported by gri38 in https://github.com/boostorg/serialization/issues/183
// and by ckreatsoMW in https://github.com/boostorg/serialization/issues/329.
// Thanks!

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <memory>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::remove;
}
#endif

#include "test_tools.hpp"

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "dll_tracking.hpp"

int test_main(int /* argc */, char * /* argv */ []){
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    dll_tracked_base base;
    base.i = 1;
    std::shared_ptr<dll_tracked_derived> first =
        std::make_shared<dll_tracked_derived>();
    std::shared_ptr<dll_tracked_derived> second =
        std::make_shared<dll_tracked_derived>();
    second->i = 2;
    second->j = 3;
    std::shared_ptr<dll_untracked_derived> other =
        std::make_shared<dll_untracked_derived>();
    other->i = 4;
    other->j = 5;
    std::shared_ptr<dll_untracked_base> through_base = other;
    {
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << BOOST_SERIALIZATION_NVP(base);
        oa << BOOST_SERIALIZATION_NVP(first);
        oa << BOOST_SERIALIZATION_NVP(second);
        oa << boost::serialization::make_nvp("again", second);
        oa << BOOST_SERIALIZATION_NVP(through_base);
    }

    dll_tracked_base base_in;
    std::shared_ptr<dll_tracked_derived> first_in;
    std::shared_ptr<dll_tracked_derived> second_in;
    std::shared_ptr<dll_tracked_derived> again_in;
    std::shared_ptr<dll_untracked_base> through_base_in;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> boost::serialization::make_nvp("base", base_in);
        ia >> boost::serialization::make_nvp("first", first_in);
        ia >> boost::serialization::make_nvp("second", second_in);
        ia >> boost::serialization::make_nvp("again", again_in);
        ia >> boost::serialization::make_nvp("through_base", through_base_in);
    }
    BOOST_CHECK(1 == base_in.i);
    BOOST_CHECK(second_in && 2 == second_in->i && 3 == second_in->j);
    BOOST_CHECK(again_in == second_in);
    const std::shared_ptr<dll_untracked_derived> other_in =
        std::dynamic_pointer_cast<dll_untracked_derived>(through_base_in);
    BOOST_CHECK(other_in && 4 == other_in->i && 5 == other_in->j);

    std::remove(testfile);
    return EXIT_SUCCESS;
}
