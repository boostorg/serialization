/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_overloaded_operators.cpp: ensure that overwriting operator<< and operator>> on global scope will not affect serialization

// (C) Copyright 2019 Kevin Leonardic
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// note: this is a compile only test.

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/static_assert.hpp>

#include <sstream>

struct non_native_type
{
    int v;
};
struct non_native_type_split
{
    int v;
};

template<class Type>
struct always_false
{
    const static bool value = false;
};

template<typename Archive>
Archive& operator>>(Archive& is, non_native_type_split& x) {
	BOOST_STATIC_ASSERT(always_false<Archive>::value);
	return is;
}
template<typename Archive>
Archive& operator<<(Archive& os, const non_native_type_split& x) {
	BOOST_STATIC_ASSERT(always_false<Archive>::value);
	return os;
}

template<typename Archive> 
Archive& operator>>(Archive& is, non_native_type& x) {
	BOOST_STATIC_ASSERT(always_false<Archive>::value);
	return is;
}
template<typename Archive> 
Archive& operator<<(Archive& os, const non_native_type& x) {
	BOOST_STATIC_ASSERT(always_false<Archive>::value);
	return os;
}

namespace boost {
	namespace serialization {
		template<class Archive>
		void load(Archive& ar, non_native_type_split& c, const unsigned int)
		{
			ar >> c.v;
		}
		template<class Archive>
		void save(Archive& ar, const non_native_type_split& c, const unsigned int)
		{
			ar << c.v;
		}
		template<class Archive>
		void serialize(Archive& ar, non_native_type& c, const unsigned int)
		{
			ar& c.v;
		}
	}
}

int main(int /* argc */, char * /* argv */[]){
    non_native_type nn;
    non_native_type_split nns;

    std::stringstream sb;
    boost::archive::binary_oarchive oa(sb);
    oa << nn;
    oa << nns;
    boost::archive::binary_iarchive ia(sb);
    ia >> nn;
    ia >> nns;

    return 0;
}

BOOST_SERIALIZATION_SPLIT_FREE(non_native_type_split)
