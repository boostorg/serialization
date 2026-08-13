/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_duplicate_type_registration.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// Every module linked against the library registers the types it serializes,
// so the type registry holds one entry per module per type.  A module going
// away has to take its own entry with it and leave the others alone, or
// serialization stops working in the modules still running.

// Reported by gast128 in
// https://github.com/boostorg/serialization/issues/325, from two COM
// components in separate DLLs registering the same type: unloading one of
// them made the other throw unregistered_class.  Both the diagnosis and the
// remedy in the report were right.  Thanks for the careful write up!

// singleton<T> explicitly allows a class derived from it to be instantiated
// more than once, which is what stands in for the second module below.

#include <cstddef>

// The lightweight test of Boost.Core is used here in place of
// test_tools.hpp, whose main locks the singleton module while the test runs,
// so that the type registry cannot change.  Loading and unloading a module
// changes it while the program runs, which is the very thing under test.
#include <boost/core/lightweight_test.hpp>

#include <boost/serialization/extended_type_info_typeid.hpp>
#include <boost/serialization/singleton.hpp>

struct base {
    virtual ~base(){}
};

struct derived : base {
};

typedef boost::serialization::extended_type_info_typeid<derived> eti_derived;

int
main(){
    const eti_derived & first =
        boost::serialization::singleton<eti_derived>::get_const_instance();

    const derived d;
    BOOST_TEST(NULL != first.get_derived_extended_type_info(d));

    {
        // A second module registers the same type.
        const eti_derived second;
        BOOST_TEST(NULL != first.get_derived_extended_type_info(d));
        BOOST_TEST(NULL != second.get_derived_extended_type_info(d));
    }
    // The second module is unloaded here.

    BOOST_TEST(NULL != first.get_derived_extended_type_info(d));

    return boost::report_errors();
}
