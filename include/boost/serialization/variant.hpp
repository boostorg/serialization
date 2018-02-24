#ifndef BOOST_SERIALIZATION_VARIANT_HPP
#define BOOST_SERIALIZATION_VARIANT_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// variant.hpp - non-intrusive serialization of variant types
//
// copyright (c) 2005   
// troy d. straszheim <troy@resophonic.com>
// http://www.resophonic.com
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org for updates, documentation, and revision history.
//
// thanks to Robert Ramey, Peter Dimov, and Richard Crossley.
//

#include <boost/assert.hpp>

#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/empty.hpp>

#include <boost/serialization/throw_exception.hpp>

#include <boost/variant.hpp>

#include <boost/archive/archive_exception.hpp>

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

namespace boost {
namespace serialization {

template<class Archive>
struct variant_save_visitor : 
    boost::static_visitor<> 
{
    variant_save_visitor(Archive& ar) :
        m_ar(ar)
    {}
    template<class T>
    void operator()(T const & value) const
    {
        m_ar << BOOST_SERIALIZATION_NVP(value);
    }
private:
    Archive & m_ar;
};

template<class Archive, BOOST_VARIANT_ENUM_PARAMS(/* typename */ class T)>
void save(
    Archive & ar,
    boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> const & v,
    unsigned int /*version*/
){
    int which = v.which();
    ar << BOOST_SERIALIZATION_NVP(which);
    variant_save_visitor<Archive> visitor(ar);
    v.apply_visitor(visitor);
}

template<class S>
struct variant_impl {

    struct action_null {
        template<class Archive, class V, class F>
        static void invoke(
            Archive & /*ar*/,
            int /*which*/,
            V & /*v*/,
            F /*f*/
        ){}
    };

    struct action_impl {
        template<class Archive, class V, class F>
        static void invoke(
            Archive & ar,
            int which,
            V & v,
            F f
        ){
            if(which == 0){
                typedef typename mpl::front<S>::type head_type;
                f.template operator()<head_type>(ar, v);
                return;
            }
            typedef typename mpl::pop_front<S>::type type;
            variant_impl<type>::action(ar, which - 1, v, f);
        }
    };

    template<class Archive, class V, class F>
    static void action(
        Archive & ar,
        int which,
        V & v,
        F f
    ){
        typedef typename mpl::eval_if<mpl::empty<S>,
            mpl::identity<action_null>,
            mpl::identity<action_impl>
        >::type typex;
        typex::invoke(ar, which, v, f);
    }

};

} // namespace serialization
    
namespace archive {
namespace detail {
        
    template<typename VFrom>
    struct reset_variant_content_address
    {
        const VFrom* vfrom;
        template<typename T, typename Archive, typename V>
        void operator()(Archive & ar, V & v) const {
            ar.reset_object_address(& boost::get<T>(v), boost::get<T>(vfrom));
        }
    };
        
    template<typename Archive,
             BOOST_VARIANT_ENUM_PARAMS(/* typename */ class T)>
    inline void reset_object_address(
        Archive & ar,
        const boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> * new_address,
        const boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> * old_address
    ){
        ar.reset_object_address(new_address, old_address);
        typedef typename boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>::types
            types;
        BOOST_ASSERT(new_address->which() == old_address->which());
        serialization::variant_impl<types>::action
            (ar, new_address->which(), *new_address,
             reset_variant_content_address
             <boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>>
             {old_address});
    }
    
} // namespace detail
} // namespace archive

namespace serialization {    

struct assign_to_variant
{
    template<typename T, typename Archive, typename V>
    void operator()(Archive & ar, V & v) const
    {
        // note: A non-intrusive implementation (such as this one)
        // necessary has to copy the value.  This wouldn't be necessary
        // with an implementation that de-serialized to the address of the
        // aligned storage included in the variant.
        T value;
        ar >> BOOST_SERIALIZATION_NVP(value);
        v = value;
        ar.reset_object_address(& boost::get<T>(v), & value);
    }
};

template<class Archive, BOOST_VARIANT_ENUM_PARAMS(/* typename */ class T)>
void load(
    Archive & ar, 
    boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>& v,
    const unsigned int version
){
    int which;
    typedef typename boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>::types types;
    ar >> BOOST_SERIALIZATION_NVP(which);
    if(which >=  mpl::size<types>::value)
        // this might happen if a type was removed from the list of variant types
        boost::serialization::throw_exception(
            boost::archive::archive_exception(
                boost::archive::archive_exception::unsupported_version
            )
        );
    variant_impl<types>::action(ar, which, v, assign_to_variant{});
}

template<class Archive,BOOST_VARIANT_ENUM_PARAMS(/* typename */ class T)>
inline void serialize(
    Archive & ar,
    boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> & v,
    const unsigned int file_version
){
    split_free(ar,v,file_version);
}

} // namespace serialization
} // namespace boost

#endif //BOOST_SERIALIZATION_VARIANT_HPP
