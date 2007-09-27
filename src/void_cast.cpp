/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// void_cast.cpp: implementation of run-time casting of void pointers

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// <gennadiy.rozental@tfn.com>

//  See http://www.boost.org for updates, documentation, and revision history.
 
#if (defined _MSC_VER) && (_MSC_VER == 1200)
# pragma warning (disable : 4786) // too long name, harmless warning
#endif

#include <cassert>

// STL
#include <set>
#include <functional>
#include <algorithm>
#include <cassert>

// BOOST
#include <boost/detail/lightweight_mutex.hpp>

#define BOOST_SERIALIZATION_SOURCE
#include <boost/serialization/extended_type_info.hpp>
#include <boost/serialization/void_cast.hpp>

namespace boost { 
namespace serialization {
namespace void_cast_detail {

// just used as a search key
class void_caster_argument : public void_caster
{
    virtual void const*
    upcast( void const* t ) const {
        assert(false);
        return NULL;
    }
    virtual void const*
    downcast( void const* t ) const {
        assert(false);
        return NULL;
    }
public:
    void_caster_argument(
        extended_type_info const& derived_type_,
        extended_type_info const& base_type_
    ) :
        void_caster(derived_type_, base_type_)
    {}
};

//////////////////////////////////////////////////////
// void_caster implementation for indirect inheritance

class void_caster_derived : public void_caster
{
    std::ptrdiff_t difference;
    virtual void const*
    upcast( void const* t ) const{
        return static_cast<const char*> ( t ) + difference;
    }
    virtual void const*
    downcast( void const* t ) const{
        return static_cast<const char*> ( t ) - difference;
    }
public:
    void_caster_derived(
        extended_type_info const& derived_type_,
        extended_type_info const& base_type_,
        std::ptrdiff_t difference_
    ) :
        void_caster(derived_type_, base_type_),
        difference( difference_ )
    {}
};

////////////////////////////////////////////////////////////
// registry to hold all casters for all base/derived pairs
// used.

class void_caster_registry
{
    struct void_caster_compare
    {
        bool
        operator()(
            const void_caster * lhs, 
            const void_caster * rhs 
        ) const {
            return *lhs < *rhs;
        }
    };
    // note usage of multi-set since it's possible that dynamically
    // loaded libraries might create multiple instances of a given
    // instance of a void caster.  This implements automatic counting
    typedef std::multiset<const void_caster *, void_caster_compare> set_type;
    set_type m_set;
public:
    typedef set_type::iterator iterator;
    typedef set_type::const_iterator const_iterator;
    void
    insert(const void_caster * vcp){
        m_set.insert(vcp);
    }
    void
    purge(const void_caster * vcp){
        iterator it = m_set.find(vcp);
        // expect it to be in there ! but check anyway !
        assert(it != m_set.end());
        m_set.erase(it);
    }
    void const *
    upcast(
        extended_type_info const & derived_type,
        extended_type_info const & base_type,
        void const * const t,
        bool top
    );
    void const *
    downcast(
        const extended_type_info & derived_type,
        const extended_type_info & base_type,
        const void * const t,
        bool top
    );
};

inline void const *
void_caster_registry::upcast(
    extended_type_info const & derived_type,
    extended_type_info const & base_type,
    void const * const t,
    bool top
){
    // same types - trivial case
    if (derived_type == base_type)
        return t;
    
    // check to see if base/derived pair is found in the registry
    void_caster_argument ca(derived_type, base_type );
    const_iterator it;
    it = m_set.find( &ca );
    
    const void * t_new = NULL;

    // if so
    if (it != m_set.end())
        // we're done
        return (*it)->upcast(t);

    // try to find a chain that gives us what we want
    for(
        it = m_set.begin();
        it != m_set.end();
        ++it
    ){
        // if the current candidate doesn't cast to the desired target type
        if ((*it)->m_base_type == base_type){
            // if the current candidate casts from the desired source type
            if ((*it)->m_derived_type == derived_type){
                // we have a base/derived match - we're done
                // cast to the intermediate type
                t_new = (*it)->upcast(t);
                break;
            }
            t_new = void_upcast(derived_type, (*it)->m_derived_type, t, false);
            if (NULL != t_new){
                t_new = (*it)->upcast(t_new);
                assert(NULL != t_new);
                if(top){
                    // register the this pair so we will have to go through
                    // keep this expensive search process more than once.
                    const void_caster * vcp = 
                        new void_caster_derived( 
                            derived_type,
                            base_type,
                            static_cast<const char*>(t_new) - static_cast<const char*>(t)
                        );
                    m_set.insert(vcp);
                }
                break;
            }
        }
    }
    return t_new;
}

inline void const *
void_caster_registry::downcast(
    const extended_type_info & derived_type,
    const extended_type_info & base_type,
    const void * const t,
    bool top
){
    // same types - trivial case
    if (derived_type == base_type)
        return t;
    
    // check to see if base/derived pair is found in the registry
    void_caster_argument ca(derived_type, base_type );
    const_iterator it;
    it = m_set.find( &ca );
    
    // if so
    if (it != m_set.end())
        // we're done
        return (*it)->downcast(t);

    const void * t_new = NULL;
    // try to find a chain that gives us what we want
    for(
        it = m_set.begin();
        it != m_set.end();
        ++it
    ){
        // if the current candidate doesn't casts from the desired target type
        if ((*it)->m_derived_type == derived_type){
            // if the current candidate casts to the desired source type
            if ((*it)->m_base_type == base_type){
                // we have a base/derived match - we're done
                // cast to the intermediate type
                t_new = (*it)->downcast(t);
                break;
            }
            t_new = void_downcast((*it)->m_base_type, base_type, t, false);
            if (NULL != t_new){
                t_new = (*it)->downcast(t_new);
                assert(NULL != t_new);
                if(top){
                    // register the this pair so we will have to go through
                    // keep this expensive search process more than once.
                    const void_caster * vcp = 
                        new void_caster_derived( 
                            derived_type,
                            base_type,
                            static_cast<const char*>(t) - static_cast<const char*>(t_new)
                        );
                    m_set.insert(vcp);
                }
                break;
            }
        }
    }
    return t_new;
}

// the above structer is fine - except for:
//     - its not thread-safe
//     - it doesn't support the necessary initialization
//       to be a singleton.
//
// Here we add the sauce to address this

class safe_void_caster_registry {
    // this addresses a problem.  Our usage patter for a typical case is:
    // void_caster
    // void_caster_register
    //     void_caster_registry
    //     insert item
    // ...
    // ~void_caster
    //     purge item
    //  ~void_caster_registry
    //  ~void_caster // crash!! void_caster_registry already deleted
    safe_void_caster_registry (){
        ++count;
    }
    ~safe_void_caster_registry (){
        --count;
    }
    static short int count;

    static boost::detail::lightweight_mutex &
    get_mutex(){
        static boost::detail::lightweight_mutex m;
        return m;
    }
    static void_caster_registry & get_instance(){
        static void_caster_registry m;
        return m;
    }
public:
    static void
    insert(const void_caster * vcp){
        boost::detail::lightweight_mutex::scoped_lock sl(get_mutex());
        get_instance().insert(vcp);
    }
    static void const *
    upcast(
        extended_type_info const & derived_type,
        extended_type_info const & base_type,
        void const * const t,
        bool top
    ){
        boost::detail::lightweight_mutex::scoped_lock sl(get_mutex());
        return get_instance().upcast(derived_type, base_type, t, top);
    }
    static void const *
    downcast(
        const extended_type_info & derived_type,
        const extended_type_info & base_type,
        const void * const t,
        bool top
    ){
        boost::detail::lightweight_mutex::scoped_lock sl(get_mutex());
        return get_instance().downcast(derived_type, base_type, t, top);
    }
    static void
    purge(const void_caster * vcp){
        boost::detail::lightweight_mutex::scoped_lock sl(get_mutex());
        if(count == 0)
            return;
        get_instance().purge(vcp);
    }
};

short int safe_void_caster_registry::count = 0;

////////////////////////////////////////////////
// void_caster implementation

BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY())
void_caster::void_caster(
    extended_type_info const & derived_type_,
    extended_type_info const & base_type_ 
) :
    m_derived_type( derived_type_),
    m_base_type(base_type_)
{}

BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY())
void_caster::~void_caster(){
    safe_void_caster_registry::purge(this);
}

void BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY())
void_caster::static_register(const void_caster * vcp) 
{
    safe_void_caster_registry::insert(vcp);
}

BOOST_SERIALIZATION_DECL(bool)
operator<(const void_caster & lhs, const void_caster & rhs){
    if( lhs.m_derived_type < rhs.m_derived_type )
        return true;

    if( rhs.m_derived_type < lhs.m_derived_type)
        return false;

    if( lhs.m_base_type < rhs.m_base_type )
        return true;

    return false;
}

} // namespace void_cast_detail

// Given a void *, assume that it really points to an instance of one type
// and alter it so that it would point to an instance of a related type.
// Return the altered pointer. If there exists no sequence of casts that
// can transform from_type to to_type, return a NULL.  

BOOST_SERIALIZATION_DECL(void const *)
void_upcast(
    extended_type_info const & derived_type,
    extended_type_info const & base_type,
    void const * const t,
    bool top
){
    return void_cast_detail::safe_void_caster_registry::upcast(
        derived_type, base_type, t, top
    );
}

BOOST_SERIALIZATION_DECL(void const *)
void_downcast(
    const extended_type_info & derived_type,
    const extended_type_info & base_type,
    const void * const t,
    bool top
){
    return void_cast_detail::safe_void_caster_registry::downcast(
        derived_type, base_type, t, top
    );
}

} // namespace serialization
} // namespace boost

