/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// dll_tracking.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define DLL_TRACKING_EXPORT

#include <boost/archive/polymorphic_iarchive.hpp>
#include <boost/archive/polymorphic_oarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include "dll_tracking.hpp"

template<class Archive>
void dll_tracked_base::serialize(Archive & ar, const unsigned int){
    ar & BOOST_SERIALIZATION_NVP(i);
}

template<class Archive>
void dll_tracked_derived::serialize(Archive & ar, const unsigned int){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(dll_tracked_base);
    ar & BOOST_SERIALIZATION_NVP(j);
}

template<class Archive>
void dll_untracked_base::serialize(Archive & ar, const unsigned int){
    ar & BOOST_SERIALIZATION_NVP(i);
}

template<class Archive>
void dll_untracked_derived::serialize(Archive & ar, const unsigned int){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(dll_untracked_base);
    ar & BOOST_SERIALIZATION_NVP(j);
}

#define DLL_TRACKING_INSTANTIATE(T)                                    \
    template DLL_TRACKING_DECL void T::serialize(                      \
        boost::archive::polymorphic_oarchive &, const unsigned int);   \
    template DLL_TRACKING_DECL void T::serialize(                      \
        boost::archive::polymorphic_iarchive &, const unsigned int);

DLL_TRACKING_INSTANTIATE(dll_tracked_base)
DLL_TRACKING_INSTANTIATE(dll_tracked_derived)
DLL_TRACKING_INSTANTIATE(dll_untracked_base)
DLL_TRACKING_INSTANTIATE(dll_untracked_derived)

BOOST_CLASS_EXPORT_IMPLEMENT(dll_tracked_base)
BOOST_CLASS_EXPORT_IMPLEMENT(dll_untracked_derived)
