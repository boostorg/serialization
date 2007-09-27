#ifndef BOOST_SERIALIZATION_EXTENDED_TYPE_INFO_TYPEID_HPP
#define BOOST_SERIALIZATION_EXTENDED_TYPE_INFO_TYPEID_HPP
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

// extended_type_info_typeid.hpp: implementation for version that depends
// on runtime typing (rtti - typeid) but uses a user specified string
// as the portable class identifier.

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <typeinfo>
#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

//#include <boost/static_warning.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <boost/serialization/extended_type_info.hpp>

#include <boost/config/abi_prefix.hpp> // must be the last header
#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4251 4231 4660 4275)
#endif

namespace boost {
namespace serialization {
namespace detail {

class BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY()) extended_type_info_typeid_0 : 
    public extended_type_info
{
protected:
    const std::type_info * m_ti;
    extended_type_info_typeid_0() :
        m_ti(NULL)
    {}
    ~extended_type_info_typeid_0();
    void type_register(const std::type_info & ti);
    static const extended_type_info *
    get_derived_extended_type_info(const std::type_info & ti);
public:
    const std::type_info & get_typeid() const {
        return *m_ti;
    }
};

///////////////////////////////////////////////////////////////////////////////
// layer to fold T and const T into the same table entry.
template<class T>
class extended_type_info_typeid_1 : 
    public detail::extended_type_info_typeid_0
{
protected:
    extended_type_info_typeid_1() :
        detail::extended_type_info_typeid_0()
    {
        type_register(typeid(T));
    }
#if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x560))
public:
#endif
    ~extended_type_info_typeid_1(){}
public:
    static const extended_type_info *
    get_derived_extended_type_info(const T & t){
        // note: this implementation - based on usage of typeid (rtti)
        // only works if the class has at least one virtual function.
//      BOOST_STATIC_WARNING(
//          static_cast<bool>(is_polymorphic::value)
//      );
        return 
            detail::extended_type_info_typeid_0::get_derived_extended_type_info(
                typeid(t)
            );
    }
    static extended_type_info *
    find(){
        // is this thread safe? probably not - does it need to be?
        static extended_type_info_typeid_1<T> instance;
        return & instance;
    }
};

} // namespace detail

///////////////////////////////////////////////////////////////////////////////
template<class T>
class extended_type_info_typeid : 
    public detail::extended_type_info_typeid_1<const T>
{
public:
    static const extended_type_info *
    get_derived_extended_type_info(const T & t){
        return detail::extended_type_info_typeid_1<T>
            ::get_derived_extended_type_info(t);
    }
    static extended_type_info *
    find(){
        // is this thread safe? probably not - does it need to be?
        return detail::extended_type_info_typeid_1<T>::find();
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
            boost::serialization::extended_type_info_typeid<T> type;
    };
    } // namespace serialization
    } // namespace boost
#endif

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
#include <boost/config/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // BOOST_SERIALIZATION_EXTENDED_TYPE_INFO_TYPEID_HPP
