#ifndef BOOST_SERIALIZATION_STD_VARIANT_HPP
#define BOOST_SERIALIZATION_STD_VARIANT_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// std_variant.hpp - deprecated, forwards to variant.hpp
//
// copyright (c) 2019 Samuel Debionne, ESRF
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org for updates, documentation, and revision history.
//

// variant.hpp serializes std::variant, next to boost::variant and
// boost::variant2::variant, and defines the same save, load and serialize
// this header used to, so the two could not both be included.  Nothing is
// left here but the forwarding, which keeps code including this header
// working and lets it include variant.hpp as well.

#include <boost/config/header_deprecated.hpp>

BOOST_HEADER_DEPRECATED("<boost/serialization/variant.hpp>")

#include <boost/serialization/variant.hpp>

#endif // BOOST_SERIALIZATION_STD_VARIANT_HPP
