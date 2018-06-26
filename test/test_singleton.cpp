/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_singleton.cpp: test implementation of the singleton template
//
// - get_[const_]_instance returns the same instance everytime it is called
// - is_destroyed returns false when singleton is active or uninitialized
// - is_destroyed returns true when singleton is destructed
// - the singleton is eventually destructed (no memory leak)

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// <gennadiy.rozental@tfn.com>

#include "test_tools.hpp"
#include "test_singleton.hpp"
#include <boost/serialization/singleton.hpp>

template<class T>
void
test1(const T & x1, const T & x2){
    BOOST_CHECK(& x1 == & x2);
}

void test_same_instance(){
    test1(
        boost::serialization::singleton<plainSingleton>::get_const_instance(),
        boost::serialization::singleton<plainSingleton>::get_const_instance()
    );
    test1(
        inheritedSingleton::get_const_instance(),
        inheritedSingleton::get_const_instance()
    );
};

int
test_main( int /* argc */, char* /* argv */[] )
{
    test_same_instance();
    // Check if the singletons are alive and use them
    BOOST_CHECK(!boost::serialization::singleton<plainSingleton>::is_destroyed());
    // For inherited we can use both: singleton<foo>::bar and foo::bar
    BOOST_CHECK(!boost::serialization::singleton<inheritedSingleton>::is_destroyed());
    BOOST_CHECK(!inheritedSingleton::is_destroyed());

    BOOST_CHECK(boost::serialization::singleton<plainSingleton>::get_const_instance().i == 42);
    BOOST_CHECK(inheritedSingleton::get_const_instance().i == 42);
    return EXIT_SUCCESS;
}

