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
#include <boost/detail/lightweight_mutex.hpp>

#define BOOST_SERIALIZATION_SOURCE
#include <boost/serialization/extended_type_info_typeid.hpp>

namespace boost { 
namespace serialization { 
namespace detail {

// map for finding the unique global extended type entry for a given type
class tkmap {
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
    // the reason that we use multiset rather than set is that its possible
    // that multiple eti records will be created as DLLS that use the same
    // eti are loaded.  Using a multset will automatically keep track of the
    // times this occurs so that when the last dll is unloaded, the type will
    // become "unregistered"
    typedef std::multiset<
        const extended_type_info_typeid_0 *,
        type_compare
    > type;
    
    type m_map;

public:
    void
    insert(const extended_type_info_typeid_0 * eti){
        m_map.insert(eti);
    }
    const extended_type_info_typeid_0 * 
    find(const extended_type_info_typeid_0 * eti){
        tkmap::type::const_iterator it;
        it = m_map.find(eti);
        if(it == m_map.end())
            return NULL;
        return *it;
    }
    void 
    purge(const extended_type_info_typeid_0 * eti){
        tkmap::type::iterator it;
        it = m_map.find(eti);
        if(it != m_map.end())
            m_map.erase(it);
    }
};

class safe_tkmap {
    // this addresses a problem.  Our usage patter for a typical case is:
    // extended_type_info_typeid
    // type_register
    //     tkmap
    //     insert item
    // ...
    // ~extended_type_info_typeid
    //     purge item
    //  tkmap
    //  ~extended_type_info_typeid // crash!! tkmap already deleted
    safe_tkmap(){
        ++count;
    }
    ~safe_tkmap(){
        --count;
    }
    static short int count;

    static boost::detail::lightweight_mutex &
    get_mutex(){
        static boost::detail::lightweight_mutex m;
        return m;
    }
    static tkmap & get_instance(){
        static tkmap m;
        return m;
    }
public:
    static void
    insert(const extended_type_info_typeid_0 * eti){
        boost::detail::lightweight_mutex::scoped_lock sl(get_mutex());
        get_instance().insert(eti);
    }
    static const extended_type_info_typeid_0 * 
    find(const extended_type_info_typeid_0 * eti){
        boost::detail::lightweight_mutex::scoped_lock sl(get_mutex());
        return get_instance().find(eti);
    }
    static void 
    purge(const extended_type_info_typeid_0 * eti){
        if(0 == count)
            return;
        boost::detail::lightweight_mutex::scoped_lock sl(get_mutex());
        get_instance().purge(eti);
    }
};

short int safe_tkmap::count = 0;

BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY()) 
extended_type_info_typeid_0::~extended_type_info_typeid_0()
{
    if(NULL == m_ti)
        return;
    // remove entries in maps which correspond to this type
    BOOST_TRY{
        detail::safe_tkmap::purge(this);
    }
    BOOST_CATCH(...){}
    BOOST_CATCH_END
}

BOOST_SERIALIZATION_DECL(void) 
extended_type_info_typeid_0::type_register(const std::type_info & ti){
    m_ti = & ti;
    detail::safe_tkmap::insert(this);
}

// this derivation is used for creating search arguments
class extended_type_info_typeid_arg : 
    public extended_type_info_typeid_0
{
public:
    extended_type_info_typeid_arg(const std::type_info & ti)
    { 
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
    return safe_tkmap::find(& etia);
}

} // namespace detail
} // namespace serialization
} // namespace boost
