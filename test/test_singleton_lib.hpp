/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// (C) Copyright 2018 Alexander Grund
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SERIALIZATION_TEST_SINGLETON_LIB_HPP
#define BOOST_SERIALIZATION_TEST_SINGLETON_LIB_HPP

#include <boost/config.hpp>

#ifdef BOOST_SERIALIZATION_TEST_SOURCE
#define BOOST_SERIALIZATION_TEST_DECL BOOST_SYMBOL_EXPORT
#else
#define BOOST_SERIALIZATION_TEST_DECL BOOST_SYMBOL_IMPORT
#endif

// Simple method that should return result
BOOST_SERIALIZATION_TEST_DECL int singleton_lib_query(int result);

#endif // BOOST_SERIALIZATION_TEST_SINGLETON_LIB_HPP
