/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// binary_iarchive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <istream>

#define BOOST_ARCHIVE_SOURCE
#include <boost/serialization/config.hpp>
#include <boost/archive/portable_iarchive.hpp>
#include <boost/archive/detail/archive_serializer_map.hpp>

#include <boost/archive/impl/archive_serializer_map.ipp>
#include <boost/archive/impl/basic_binary_iarchive.ipp>
#include <boost/archive/impl/basic_binary_iprimitive.ipp>

namespace boost {
namespace archive {


	// explicitly instantiate for this type of binary stream
	template class basic_binary_iarchive<portable_iarchive>;

	template class basic_binary_iprimitive<
		portable_iarchive
		, std::istream::char_type
		, std::istream::traits_type
	>;

	// need to instantiate this template also for polymorphic version?
	template class detail::archive_serializer_map<portable_iarchive>;
	template class detail::archive_serializer_map<polymorphic_portable_iarchive>;

} // namespace archive
} // namespace boost
