/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// extended_type_info.cpp: implementation for portable version of type_info

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#if (defined _MSC_VER) && (_MSC_VER == 1200)
#  pragma warning (disable : 4786) // too long name, harmless warning
#endif

#include <algorithm>
#include <set>
#include <utility>
#include <cassert>

#include <boost/config.hpp> // msvc needs this to suppress warning

#include <cstring>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ using ::strcmp; }
#endif

#include <boost/detail/no_exceptions_support.hpp>
#include <boost/serialization/singleton.hpp>
#include <boost/serialization/force_include.hpp>

#define BOOST_SERIALIZATION_SOURCE
#include <boost/serialization/extended_type_info.hpp>

namespace boost { 
namespace serialization {
namespace detail {

struct key_compare
{
    bool
    operator()(
        const extended_type_info * lhs, 
        const extended_type_info * rhs
    ) const {
        return *lhs < *rhs;
    }
};
typedef std::set<
    const extended_type_info *, 
    key_compare
> ktmap;

template ktmap;

class extended_type_info_arg : public extended_type_info
{
public:
    extended_type_info_arg(const char * key){
        m_key = key;
    }
    ~extended_type_info_arg(){
        m_key = NULL;
    }
};

} // namespace detail

BOOST_SERIALIZATION_DECL(void)  
extended_type_info::key_register(const char *key) {
    assert(NULL != key);
    m_key = key;
    std::pair<detail::ktmap::const_iterator, bool> result;
    // prohibit duplicates and multiple registrations
    result = singleton<detail::ktmap>::get_mutable_instance().insert(this);
    assert(result.second);
    // would like to throw and exception here but I don't
    // have one conveniently defined
    // throw(?)
}

BOOST_SERIALIZATION_DECL(const extended_type_info *) 
extended_type_info::find(const char *key) {
    const detail::ktmap & k = singleton<detail::ktmap>::get_const_instance();
    const detail::extended_type_info_arg eti_key(key);
    const detail::ktmap::const_iterator it = k.find(& eti_key);
    if(k.end() == it)
        return NULL;
    return *(it);
}

extended_type_info::extended_type_info() : 
    m_key(NULL)
{
    // make sure that the ktmap is instantiated before 
    // the first key is added to it.
    singleton<detail::ktmap>::get_const_instance();
}

BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY()) 
extended_type_info::~extended_type_info(){
    if(NULL != m_key){
        unsigned int erase_count;
        erase_count = 
            singleton<detail::ktmap>::get_mutable_instance().erase(this);
        assert(1 == erase_count);
    }
}

BOOST_SERIALIZATION_DECL(bool)  
operator==(
    const extended_type_info & lhs, 
    const extended_type_info & rhs
){
    return (& lhs == & rhs);
}

BOOST_SERIALIZATION_DECL(bool)
operator<(
    const extended_type_info & lhs, 
    const extended_type_info & rhs
){
    // shortcut to exploit string pooling
    const char * l = lhs.get_key();
    const char * r = rhs.get_key();
    // neither have been exported
    if(NULL == l && NULL == r)
        // order by address
        return & lhs < & rhs;
    // exported types are "higher" than non-exported types
    if(NULL == l)
        return true;
    if(NULL == r)
        return false;
    // for exported types, use the string key so that
    // multiple instances in different translation units
    // can be matched up
    return -1 == std::strcmp(l, r); 
}

} // namespace serialization
} // namespace boost
