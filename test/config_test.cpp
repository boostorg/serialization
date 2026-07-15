/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// config_test.cpp

// Copyright 2014 Robert Ramey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

#include "../../../boost/config.hpp"

#if defined(__clang__)
#pragma message "__clang__ defined"
#endif

#if defined(BOOST_CLANG)
#pragma message "BOOST_CLANG defined"
#endif

#if defined(__GNUC__)
#pragma message "__GNUC__ defined"
#endif

#include "../../../boost/mpl/print.hpp"

typedef int x;
