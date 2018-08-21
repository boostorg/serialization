/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_singleton_shared.cpp: test implementation of the singleton template
//                            with shared libraries

// (C) Copyright 2018 Alexander Grund
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "test_tools.hpp"
#include "test_singleton.hpp"
#include "test_singleton_lib.hpp"
#include <boost/serialization/singleton.hpp>

int
test_main( int /* argc */, char* /* argv */[] )
{
    // Check "our" singletons
    BOOST_CHECK(!boost::serialization::singleton<plainSingleton>::is_destroyed());
    BOOST_CHECK(!inheritedSingleton::is_destroyed());

    BOOST_CHECK(boost::serialization::singleton<plainSingleton>::get_const_instance().i == 42);
    BOOST_CHECK(inheritedSingleton::get_const_instance().i == 42);

    // Call into library to check the singletons there
    BOOST_CHECK(singleton_lib_query(1337) == 1337);
    return EXIT_SUCCESS;
}

