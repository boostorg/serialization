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
#include <cassert>

#include <boost/config.hpp> // msvc needs this to suppress warning

#include <cstring>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ using ::strcmp; }
#endif

#include <boost/detail/no_exceptions_support.hpp>
#include <boost/detail/lightweight_mutex.hpp>

#define BOOST_SERIALIZATION_SOURCE
#include <boost/serialization/extended_type_info.hpp>

namespace boost { 
namespace serialization {
namespace detail {

// map for finding the unique global extended type info entry given its GUID
class ktmap {
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
    // the reason that we use multiset rather than set is that its possible
    // that multiple eti records will be created as DLLS that use the same
    // eti are loaded.  Using a multset will automatically keep track of the
    // times this occurs so that when the last dll is unloaded, the type will
    // become "unregistered"
    typedef std::multiset<const extended_type_info *, key_compare> type;
    type m_map;
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
public:
    void
    insert(const extended_type_info * eti){
        m_map.insert(eti);
    }
    const extended_type_info * 
    find(const char *key)
    {
        const extended_type_info_arg eti(key);
        ktmap::type::iterator it;
        it = m_map.find(& eti);
        if(it == m_map.end())
            return NULL;
        return *it;
    }
    void 
    purge(const extended_type_info * eti){
        ktmap::type::iterator it;
        it = m_map.find(eti);
        // expect it to be in there ! but check anyway !
        if(it != m_map.end())
            m_map.erase(it);
    }
};

// the above structer is fine - except for:
//     - its not thread-safe
//     - it doesn't support the necessary initialization
//       to be a singleton.
//
// Here we add the sauce to address this

class safe_ktmap {
    // this addresses a problem.  Our usage patter for a typical case is:
    // extended_type_info
    // key_register
    //     ktmap
    //     insert item
    // ...
    // ~extended_type_info
    //     purge item
    //  ~ktmap
    //  ~extended_type_info // crash!! ktmap already deleted
    safe_ktmap(){
        ++count;
    }
    ~safe_ktmap(){
        --count;
    }
    static short int count;

    static boost::detail::lightweight_mutex &
    get_mutex(){
        static boost::detail::lightweight_mutex m;
        return m;
    }
    static ktmap & get_instance(){
        static ktmap m;
        return m;
    }
public:
    static void
    insert(const extended_type_info * eti){
        boost::detail::lightweight_mutex::scoped_lock sl(get_mutex());
        get_instance().insert(eti);
    }
    static const extended_type_info * 
    find(const char *key){
        boost::detail::lightweight_mutex::scoped_lock sl(get_mutex());
        return get_instance().find(key);
    }
    static void 
    purge(const extended_type_info * eti){
        if(0 == detail::safe_ktmap::count)
            return;
        boost::detail::lightweight_mutex::scoped_lock sl(get_mutex());
        get_instance().purge(eti);
    }
};

short int safe_ktmap::count = 0;

} // namespace detail

BOOST_SERIALIZATION_DECL(const extended_type_info *) 
extended_type_info::find(const char *key)
{
    return detail::safe_ktmap::find(key);
}

BOOST_SERIALIZATION_DECL(void)  
extended_type_info::key_register(const char *k) {
    assert(NULL != k);
    m_key = k;
    detail::safe_ktmap::insert(this);
}

extended_type_info::extended_type_info() : 
    m_key(NULL)
{
}

BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY()) 
extended_type_info::~extended_type_info(){
    if(NULL == m_key)
        return;
    // remove entries in maps which correspond to this type
    BOOST_TRY{
        detail::safe_ktmap::purge(this);
    }
    BOOST_CATCH(...){}
    BOOST_CATCH_END
}

BOOST_SERIALIZATION_DECL(bool)  
operator==(
    const extended_type_info & lhs, 
    const extended_type_info & rhs
){
    if(& lhs == & rhs)
        return true;
    const char * l = lhs.get_key();
    const char * r = rhs.get_key();
    // neither have been exported
    if(NULL == l && NULL == r)
        // then the above test is definitive
        return false;
    // shortcut to exploit string pooling
    if(l == r)
        return true;
    if(NULL == r)
        return false;
    if(NULL == l)
        return false;
    return 0 == std::strcmp(l, r); 
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
