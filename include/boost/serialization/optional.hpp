/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8

// (C) Copyright 2002-4 Pavel Vozenilek .
// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Provides non-intrusive serialization for boost::optional.

#ifndef BOOST_SERIALIZATION_OPTIONAL_HPP
#define BOOST_SERIALIZATION_OPTIONAL_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <boost/config.hpp>
#include <boost/optional.hpp>
#ifndef BOOST_NO_CXX17_HDR_OPTIONAL
#include <optional>
#endif

#include <boost/move/utility_core.hpp>
#include <boost/core/addressof.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/library_version_type.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/detail/is_default_constructible.hpp>
#include <boost/serialization/detail/stack_constructor.hpp>

// function specializations must be defined in the appropriate
// namespace - boost::serialization
namespace boost {
namespace serialization {
namespace detail {

// The value of an optional<T> is serialized like a single element of an
// STL container: when T is default constructible we serialize it in place,
// otherwise we route construction through save/load_construct_data so that
// types without a default constructor can be reconstructed on load.  The
// default-constructible path is left untouched so that archives written by
// earlier versions of the library keep the same layout.

// save the value: T is default constructible
template<class Archive, class OT>
typename boost::enable_if<
    typename detail::is_default_constructible<typename OT::value_type>,
    void
>::type
save_value(Archive & ar, const OT & ot){
    ar << boost::serialization::make_nvp("value", *ot);
}

// save the value: T is not default constructible
template<class Archive, class OT>
typename boost::disable_if<
    typename detail::is_default_constructible<typename OT::value_type>,
    void
>::type
save_value(Archive & ar, const OT & ot){
    typedef typename OT::value_type value_type;
    const value_type & v = *ot;
    const boost::serialization::item_version_type item_version(
        boost::serialization::version<value_type>::value
    );
    ar << BOOST_SERIALIZATION_NVP(item_version);
    boost::serialization::save_construct_data_adl(
        ar,
        boost::addressof(v),
        item_version
    );
    ar << boost::serialization::make_nvp("value", v);
}

// load the value: T is default constructible
template<class Archive, class OT>
typename boost::enable_if<
    typename detail::is_default_constructible<typename OT::value_type>,
    void
>::type
load_value(Archive & ar, OT & ot, const unsigned int version){
    if(0 == version){
        boost::serialization::item_version_type item_version(0);
        boost::serialization::library_version_type library_version(
            ar.get_library_version()
        );
        if(boost::serialization::library_version_type(3) < library_version){
            ar >> BOOST_SERIALIZATION_NVP(item_version);
        }
    }
    typename OT::value_type t;
    ar >> boost::serialization::make_nvp("value", t);
    ot = boost::move(t);
}

// load the value: T is not default constructible
template<class Archive, class OT>
typename boost::disable_if<
    typename detail::is_default_constructible<typename OT::value_type>,
    void
>::type
load_value(Archive & ar, OT & ot, const unsigned int /* version */){
    typedef typename OT::value_type value_type;
    boost::serialization::item_version_type item_version(0);
    ar >> BOOST_SERIALIZATION_NVP(item_version);
    detail::stack_construct<Archive, value_type> aux(ar, item_version);
    ar >> boost::serialization::make_nvp("value", aux.reference());
    ot = boost::move(aux.reference());
    ar.reset_object_address(boost::addressof(*ot), aux.address());
}

// OT is of the form optional<T>
template<class Archive, class OT>
void save_impl(
    Archive & ar,
    const OT & ot
){
    const bool tflag(ot);
    ar << boost::serialization::make_nvp("initialized", tflag);
    if (tflag){
        save_value(ar, ot);
    }
}

// OT is of the form optional<T>
template<class Archive, class OT>
void load_impl(
    Archive & ar,
    OT & ot,
    const unsigned int version
){
    bool tflag;
    ar >> boost::serialization::make_nvp("initialized", tflag);
    if(! tflag){
        ot.reset();
        return;
    }
    load_value(ar, ot, version);
}

} // detail

template<class Archive, class T>
void save(
    Archive & ar,
    const boost::optional< T > & ot,
    const unsigned int /*version*/
){
    detail::save_impl(ar, ot);
}

#ifndef BOOST_NO_CXX17_HDR_OPTIONAL
template<class Archive, class T>
void save(
    Archive & ar,
    const std::optional< T > & ot,
    const unsigned int /*version*/
){
    detail::save_impl(ar, ot);
}
#endif

template<class Archive, class T>
void load(
    Archive & ar,
    boost::optional< T > & ot,
    const unsigned int version
){
    detail::load_impl(ar, ot, version);
}

#ifndef BOOST_NO_CXX17_HDR_OPTIONAL
template<class Archive, class T>
void load(
    Archive & ar,
    std::optional< T >  & ot,
    const unsigned int version
){
    detail::load_impl(ar, ot, version);
}
#endif

template<class Archive, class T>
void serialize(
    Archive & ar,
    boost::optional< T > & ot,
    const unsigned int version
){
    boost::serialization::split_free(ar, ot, version);
}

#ifndef BOOST_NO_CXX17_HDR_OPTIONAL
template<class Archive, class T>
void serialize(
    Archive & ar,
    std::optional< T > & ot,
    const unsigned int version
){
    boost::serialization::split_free(ar, ot, version);
}
#endif

template<class T>
struct version<boost::optional<T> >{
    BOOST_STATIC_CONSTANT(int, value = 1);
};

#ifndef BOOST_NO_CXX17_HDR_OPTIONAL
template<class T>
struct version<std::optional<T> >{
    BOOST_STATIC_CONSTANT(int, value = 1);
};
#endif

} // serialization
} // boost

#endif // BOOST_SERIALIZATION_OPTIONAL_HPP
