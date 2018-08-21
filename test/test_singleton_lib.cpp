/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_singleton_lib.cpp: simple library using singletons to test usage in shared libraries

// (C) Copyright 2018 Alexander Grund
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_SERIALIZATION_TEST_SOURCE

#include "test_singleton_lib.hpp"
#include "test_singleton.hpp"

int singleton_lib_query(int result){
    return boost::serialization::singleton<plainSingleton>::get_const_instance().i
    	- inheritedSingleton::get_const_instance().i
    	+ result;
}

