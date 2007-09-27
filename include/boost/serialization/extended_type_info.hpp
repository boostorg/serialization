#ifndef BOOST_SERIALIZATION_EXTENDED_TYPE_INFO_HPP
#define BOOST_SERIALIZATION_EXTENDED_TYPE_INFO_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// extended_type_info.hpp: interface for portable version of type_info

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// for now, extended type info is part of the serialization libraries
// this could change in the future.
#include <boost/config.hpp>
#include <boost/noncopyable.hpp>
#include <boost/serialization/config.hpp>

#include <boost/config/abi_prefix.hpp> // must be the last header
#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4251 4231 4660 4275)
#endif

#define BOOST_SERIALIZATION_MAX_KEY_SIZE 128

namespace boost { 
namespace serialization {

class BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY()) extended_type_info : 
    private boost::noncopyable 
{
protected:
    // this class can't be used as is. It's just the 
    // common functionality for all type_info replacement
    // systems.  Hence, make these protected
    extended_type_info();
    ~extended_type_info();
    const char * m_key;
public:
    void key_register(const char *key);
    const char * get_key() const {
        return m_key;
    }
    static const extended_type_info * find(const char *key);
};

// in order
BOOST_SERIALIZATION_DECL(bool)  
operator==(
    const extended_type_info & lhs, 
    const extended_type_info & rhs
);

BOOST_SERIALIZATION_DECL(bool)  
operator<(
    const extended_type_info & lhs, 
    const extended_type_info & rhs
);

} // namespace serialization 
} // namespace boost

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
#include <boost/config/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // BOOST_SERIALIZATION_EXTENDED_TYPE_INFO_HPP

