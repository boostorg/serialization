/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_std_variant_forwarding.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// std_variant.hpp is deprecated and does nothing but include variant.hpp.
// Code which includes it has to keep working, and, unlike before, has to be
// able to include variant.hpp as well: the two used to define the same
// save, load and serialize, so including both was an error.

// The deprecation message is silenced here only to keep it out of the test
// output.  Its absence is not what is being checked.
#define BOOST_ALLOW_DEPRECATED_HEADERS

#include <boost/config.hpp>

#ifndef BOOST_NO_CXX17_HDR_VARIANT
#include <variant>
#endif

#include <boost/serialization/std_variant.hpp>
#include <boost/serialization/variant.hpp>

#include <boost/archive/text_oarchive.hpp>

int main(){
    #ifndef BOOST_NO_CXX17_HDR_VARIANT
    // reaching the serialization of std::variant through the deprecated
    // header is the point of it still being there
    const std::variant<int, char> v(1);
    (void) v;
    #endif
    return 0;
}
