/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_public_base_type.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// The classes an archive uses for versions, identifiers, counts and flags
// each wrap a single value and convert to it.  Generic code reading such a
// value has to name the wrapped type, so every one of them publishes it as
// base_type.

// Requested by smuzaffar in
// https://github.com/boostorg/serialization/issues/326.  With the typedef
// out of reach, generic code had to guess the wrapped type, and a
// static_cast to a wrong guess does not compile at all.  A C style cast
// does, by reinterpreting the object rather than converting it, which is
// what GCC 14 warned about, and rightly so.  Thanks for the report.

#include <boost/archive/basic_archive.hpp>
#include <boost/serialization/collection_size_type.hpp>
#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/library_version_type.hpp>

// What a generic reader wants to write.  A static_cast selects the
// conversion operator, so nothing is reinterpreted.
template<class T>
void check(T & t){
    typename T::base_type & r = static_cast<typename T::base_type &>(t);
    (void) r;
}

// And the same for a generic writer, through the const conversion.
template<class T>
void check_const(const T & t){
    const typename T::base_type v = static_cast<typename T::base_type>(t);
    (void) v;
}

template<class T>
void check_both(T & t){
    check(t);
    check_const(t);
}

int main(){
    boost::archive::version_type version(0);
    boost::archive::class_id_type class_id(0);
    boost::archive::object_id_type object_id(std::size_t(0));
    boost::archive::tracking_type tracking(false);
    boost::serialization::collection_size_type collection_size(0);
    boost::serialization::item_version_type item_version(0);
    boost::serialization::library_version_type library_version(0);

    check_both(version);
    check_both(class_id);
    check_both(object_id);
    check_both(tracking);
    check_both(collection_size);
    check_both(item_version);
    check_both(library_version);

    return 0;
}
