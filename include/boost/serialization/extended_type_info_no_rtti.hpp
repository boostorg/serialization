#ifndef BOOST_EXTENDED_TYPE_INFO_NO_RTTI_HPP
#define BOOST_EXTENDED_TYPE_INFO_NO_RTTI_HPP
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

// extended_type_info_no_rtti.hpp: implementation for version that depends
// on runtime typing (rtti - typeid) but uses a user specified string
// as the portable class identifier.

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
#include <cassert>
#include <boost/config.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_const.hpp>

#include <boost/serialization/extended_type_info.hpp>
#include <boost/mpl/bool.hpp>

#include <boost/config/abi_prefix.hpp> // must be the last header
#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4251 4231 4660 4275)
#endif

namespace boost {
namespace serialization {

template<class T>
class extended_type_info_no_rtti : 
    public extended_type_info
{
    // private constructor to inhibit any existence other than the 
    // static one
    extended_type_info_no_rtti(){}
    ~extended_type_info_no_rtti(){};
public:
    static const boost::serialization::extended_type_info *
    get_derived_extended_type_info(const T & t){
        // find the type that corresponds to the most derived type.
        // this implementation doesn't depend on typeid() but assumes
        // that the specified type has a function of the following signature.
        // A common implemention of such a function is to define as a virtual
        // function. 
        const char * derived_key = t.get_key();
        assert(NULL != derived_key);
        return boost::serialization::extended_type_info::find(derived_key);
    }
    static boost::serialization::extended_type_info *
    find(){
        // is this thread safe? probably not - does it need to be?
        static extended_type_info_no_rtti<T> instance;
        return & instance;
    }
};

} // namespace serialization
} // namespace boost

///////////////////////////////////////////////////////////////////////////////
// If no other implementation has been designated as default, 
// use this one.  To use this implementation as the default, specify it
// before any of the other headers.

#ifndef BOOST_SERIALIZATION_DEFAULT_TYPE_INFO
    #define BOOST_SERIALIZATION_DEFAULT_TYPE_INFO
    namespace boost {
    namespace serialization {
    template<class T>
    struct extended_type_info_impl {
        typedef BOOST_DEDUCED_TYPENAME 
            boost::serialization::extended_type_info_no_rtti<const T> type;
    };
    } // namespace serialization
    } // namespace boost
#endif

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
#include <boost/config/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // BOOST_EXTENDED_TYPE_INFO_NO_RTTI_HPP
