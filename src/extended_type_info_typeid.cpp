/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// extended_type_info_typeid.cpp: specific implementation of type info
// that is based on typeid

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <algorithm>
#include <set>
#include <cassert>
#include <typeinfo>

#include <boost/detail/no_exceptions_support.hpp>

#include <boost/serialization/singleton.hpp>

#define BOOST_SERIALIZATION_SOURCE
#include <boost/serialization/extended_type_info_typeid.hpp>

namespace boost { 
namespace serialization { 
namespace detail {

struct type_compare
{
    bool
    operator()(
        const extended_type_info_typeid_0 * lhs,
        const extended_type_info_typeid_0 * rhs
    ) const {
        const std::type_info & l = lhs->get_typeid();
        const std::type_info & r = rhs->get_typeid();
        return l.before(r);
    }
};
typedef std::set<
    const extended_type_info_typeid_0 *,
    type_compare
> tkmap;
    
template tkmap;

BOOST_SERIALIZATION_DECL(void) 
extended_type_info_typeid_0::type_register(const std::type_info & ti){
    m_ti = & ti;
    std::pair<tkmap::const_iterator, bool> result;
    result = singleton<tkmap>::get_mutable_instance().insert(this);
    assert(result.second);
    // would like to throw and exception here but I don't
    // have one conveniently defined
    // throw(?)
}

BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY()) 
extended_type_info_typeid_0::~extended_type_info_typeid_0()
{
    if(NULL != m_ti){
        // remove entries in maps which correspond to this type
        unsigned int erase_count;
        erase_count 
            = singleton<tkmap>::get_mutable_instance().erase(this);
    }
}

// this derivation is used for creating search arguments
class extended_type_info_typeid_arg : 
    public extended_type_info_typeid_0
{
public:
    extended_type_info_typeid_arg(const std::type_info & ti){ 
        // note absense of self register and key as this is used only as
        // search argument given a type_info reference and is not to 
        // be added to the map.
        m_ti = & ti;
    }
    ~extended_type_info_typeid_arg(){
        m_ti = NULL;
    }
};

BOOST_SERIALIZATION_DECL(const extended_type_info *)
extended_type_info_typeid_0::get_derived_extended_type_info(
    const std::type_info & ti
){
    detail::extended_type_info_typeid_arg etia(ti);
    const tkmap & t = singleton<tkmap>::get_const_instance();
    const tkmap::const_iterator it = t.find(& etia);
    if(t.end() == it)
        return NULL;
    return *(it);
}

} // namespace detail
} // namespace serialization
} // namespace boost
