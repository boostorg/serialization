/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_factory.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// BOOST_SERIALIZATION_FACTORY builds an object from the arguments an
// extended_type_info was asked to construct it with.  Only the arity zero
// form was ever used in the library, so the others went untested.

// Reported by ivytin in
// https://github.com/boostorg/serialization/issues/215: a misplaced
// closing parenthesis kept every arity above zero from preprocessing at
// all.  Thanks for spotting it, and for pinning down which parenthesis.

// The arguments travel through a va_list, so they are limited to what
// survives the default argument promotions.  A class type would not, which
// is why the ones below are all int and double.

#include <cstddef>
#include <memory>

#include <boost/config.hpp>
#include <boost/serialization/extended_type_info_typeid.hpp>
#include <boost/serialization/factory.hpp>
#include <boost/serialization/singleton.hpp>

#include "test_tools.hpp"

struct arg0 {
    arg0() : sum(0) {}
    int sum;
};

struct arg1 {
    explicit arg1(int a) : sum(a) {}
    int sum;
};

struct arg2 {
    arg2(int a, int b) : sum(a + b) {}
    int sum;
};

struct arg3 {
    arg3(int a, int b, int c) : sum(a + b + c) {}
    int sum;
};

struct arg4 {
    arg4(int a, int b, int c, double d)
        : sum(a + b + c + static_cast<int>(d)) {}
    int sum;
};

BOOST_SERIALIZATION_FACTORY_0(arg0)
BOOST_SERIALIZATION_FACTORY_1(arg1, int)
BOOST_SERIALIZATION_FACTORY_2(arg2, int, int)
BOOST_SERIALIZATION_FACTORY_3(arg3, int, int, int)
BOOST_SERIALIZATION_FACTORY_4(arg4, int, int, int, double)

template<class T>
static const boost::serialization::extended_type_info & eti_of(){
    return boost::serialization::singleton<
        boost::serialization::extended_type_info_typeid<T>
    >::get_const_instance();
}

// construct() hands back a raw pointer it has just newed, so adopt it
template<class T>
static std::unique_ptr<T> owned(void * p){
    return std::unique_ptr<T>(static_cast<T *>(p));
}

// construct() takes the count and then the arguments themselves, so the
// factory is reached the way the library reaches it
int
test_main(int /* argc */, char * /* argv */ [])
{
    const std::unique_ptr<arg0> a0(owned<arg0>(eti_of<arg0>().construct(0)));
    BOOST_REQUIRE(NULL != a0.get());
    BOOST_CHECK(0 == a0->sum);

    const std::unique_ptr<arg1> a1(owned<arg1>(eti_of<arg1>().construct(1, 7)));
    BOOST_REQUIRE(NULL != a1.get());
    BOOST_CHECK(7 == a1->sum);

    const std::unique_ptr<arg2> a2(
        owned<arg2>(eti_of<arg2>().construct(2, 7, 11)));
    BOOST_REQUIRE(NULL != a2.get());
    BOOST_CHECK(18 == a2->sum);

    const std::unique_ptr<arg3> a3(
        owned<arg3>(eti_of<arg3>().construct(3, 7, 11, 13)));
    BOOST_REQUIRE(NULL != a3.get());
    BOOST_CHECK(31 == a3->sum);

    const std::unique_ptr<arg4> a4(
        owned<arg4>(eti_of<arg4>().construct(4, 7, 11, 13, 2.0)));
    BOOST_REQUIRE(NULL != a4.get());
    BOOST_CHECK(33 == a4->sum);

    return EXIT_SUCCESS;
}
