#ifndef BOOST_SERIALIZATION_TEST_DLL_TRACKING_HPP
#define BOOST_SERIALIZATION_TEST_DLL_TRACKING_HPP

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// dll_tracking.hpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Two hierarchies whose serialization lives in the shared library
// dll_tracking.  The library exports dll_tracked_base, and so serializes it
// through a pointer: its serializer for that type tracks objects.  It never
// serializes dll_untracked_base through a pointer.

#include <boost/config.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>

#if defined(DLL_TRACKING_EXPORT)
    #define DLL_TRACKING_DECL BOOST_SYMBOL_EXPORT
#else
    #define DLL_TRACKING_DECL BOOST_SYMBOL_IMPORT
#endif

class BOOST_SYMBOL_VISIBLE dll_tracked_base {
    friend class boost::serialization::access;
    template<class Archive>
    DLL_TRACKING_DECL void serialize(Archive & ar, const unsigned int version);
public:
    int i = 0;
    virtual ~dll_tracked_base() {}
};

class BOOST_SYMBOL_VISIBLE dll_tracked_derived : public dll_tracked_base {
    friend class boost::serialization::access;
    template<class Archive>
    DLL_TRACKING_DECL void serialize(Archive & ar, const unsigned int version);
public:
    int j = 0;
};

class BOOST_SYMBOL_VISIBLE dll_untracked_base {
    friend class boost::serialization::access;
    template<class Archive>
    DLL_TRACKING_DECL void serialize(Archive & ar, const unsigned int version);
public:
    int i = 0;
    virtual ~dll_untracked_base() {}
};

class BOOST_SYMBOL_VISIBLE dll_untracked_derived : public dll_untracked_base {
    friend class boost::serialization::access;
    template<class Archive>
    DLL_TRACKING_DECL void serialize(Archive & ar, const unsigned int version);
public:
    int j = 0;
};

BOOST_CLASS_EXPORT_KEY(dll_tracked_base)
BOOST_CLASS_EXPORT_KEY(dll_untracked_derived)

#endif // BOOST_SERIALIZATION_TEST_DLL_TRACKING_HPP
