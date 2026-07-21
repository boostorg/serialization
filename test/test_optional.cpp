/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_optional.cpp

// (C) Copyright 2004 Pavel Vozenilek
// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>

#include <boost/config.hpp>

#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::remove;
}
#endif

#include <boost/archive/archive_exception.hpp>
#include "test_tools.hpp"


struct A {
    int m_x;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */){
        ar & boost::serialization::make_nvp("x", m_x);
    };
    bool operator==(const A & rhs) const {
        return m_x == rhs.m_x;
    }
    // note that default constructor is not trivial
    A() :
        m_x(0)
    {}
    A(int x) :
        m_x(x)
    {}
};

// Optional is the class optional implementation you use
template<template<class> class Optional>
int test(){
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    const Optional<int> aoptional1;
    const Optional<int> aoptional2(123);
    const Optional<A> aoptional3;
    A a(1);
    const Optional<A> aoptional4(a);
    const Optional<A *> aoptional5;
    const Optional<A *> aoptional6(& a);
    {
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << boost::serialization::make_nvp("aoptional1",aoptional1);
        oa << boost::serialization::make_nvp("aoptional2",aoptional2);
        oa << boost::serialization::make_nvp("aoptional3",aoptional3);
        oa << boost::serialization::make_nvp("aoptional4",aoptional4);
        oa << boost::serialization::make_nvp("aoptional5",aoptional5);
        oa << boost::serialization::make_nvp("aoptional6",aoptional6);
    }
    Optional<int> aoptional1a(999);
    Optional<int> aoptional2a;
    Optional<A> aoptional3a;
    Optional<A> aoptional4a;
    Optional<A *> aoptional5a;
    Optional<A *> aoptional6a;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> boost::serialization::make_nvp("aoptional1",aoptional1a);
        ia >> boost::serialization::make_nvp("aoptional2",aoptional2a);
        ia >> boost::serialization::make_nvp("aoptional3",aoptional3a);
        ia >> boost::serialization::make_nvp("aoptional4",aoptional4a);
        ia >> boost::serialization::make_nvp("aoptional5",aoptional5a);
        ia >> boost::serialization::make_nvp("aoptional6",aoptional6a);
    }
    BOOST_CHECK(aoptional1 == aoptional1a);
    BOOST_CHECK(aoptional2 == aoptional2a);
    BOOST_CHECK(aoptional3 == aoptional3a);
    BOOST_CHECK(aoptional4 == aoptional4a);
    BOOST_CHECK(aoptional5 == aoptional5a);  // not initialized
    BOOST_CHECK(**aoptional6 == **aoptional6a);

    std::remove(testfile);
    return EXIT_SUCCESS;
}

// A move-only value type: deleted copy, defaulted move.  Loading an
// optional<M> must move the deserialized value into place rather than copy
// it.
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES) \
    && !defined(BOOST_NO_CXX11_DELETED_FUNCTIONS) \
    && !defined(BOOST_NO_CXX11_DEFAULTED_FUNCTIONS)
#define BOOST_SERIALIZATION_TEST_OPTIONAL_MOVE_ONLY

struct M {
    int m_x;
    M() : m_x(0) {}
    explicit M(int x) : m_x(x) {}
    M(const M &) = delete;
    M & operator=(const M &) = delete;
    M(M &&) = default;
    M & operator=(M &&) = default;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */){
        ar & boost::serialization::make_nvp("x", m_x);
    }
};

template<template<class> class Optional>
int test_move_only(){
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    Optional<M> o_empty;
    Optional<M> o_value(M(42));
    {
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << boost::serialization::make_nvp("o_empty", o_empty);
        oa << boost::serialization::make_nvp("o_value", o_value);
    }
    // start each target in the opposite state, so load must both reset and
    // assign
    Optional<M> o_empty_a(M(7));
    Optional<M> o_value_a;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> boost::serialization::make_nvp("o_empty", o_empty_a);
        ia >> boost::serialization::make_nvp("o_value", o_value_a);
    }
    BOOST_CHECK(! o_empty_a);
    BOOST_CHECK(static_cast<bool>(o_value_a) && 42 == o_value_a->m_x);

    std::remove(testfile);
    return EXIT_SUCCESS;
}
#endif // move-only support available

// A type without a default constructor.  It is reconstructed on load through
// save_construct_data / load_construct_data, which is exactly what an
// optional<ND> now relies on (see issue #121).  m_i is the constructor
// argument, m_x is ordinary serialized state.
struct ND {
    int m_i;
    int m_x;
    ND(int i, int x) : m_i(i), m_x(x) {}
    explicit ND(int i) : m_i(i), m_x(0) {}
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */){
        ar & boost::serialization::make_nvp("x", m_x);
    }
    bool operator==(const ND & rhs) const {
        return m_i == rhs.m_i && m_x == rhs.m_x;
    }
};

namespace boost {
namespace serialization {

template<class Archive>
void save_construct_data(
    Archive & ar, const ND * p, const unsigned int /* version */
){
    ar << boost::serialization::make_nvp("i", p->m_i);
}

template<class Archive>
void load_construct_data(
    Archive & ar, ND * p, const unsigned int /* version */
){
    int i;
    ar >> boost::serialization::make_nvp("i", i);
    ::new(p) ND(i);
}

} // serialization
} // boost

template<template<class> class Optional>
int test_non_default_ctor(){
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    const Optional<ND> o_empty;
    const Optional<ND> o_value(ND(7, 42));
    {
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << boost::serialization::make_nvp("o_empty", o_empty);
        oa << boost::serialization::make_nvp("o_value", o_value);
    }
    // start each target in the opposite state, so load must both reset and
    // reconstruct
    Optional<ND> o_empty_a(ND(1, 1));
    Optional<ND> o_value_a;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> boost::serialization::make_nvp("o_empty", o_empty_a);
        ia >> boost::serialization::make_nvp("o_value", o_value_a);
    }
    BOOST_CHECK(! o_empty_a);
    BOOST_CHECK(static_cast<bool>(o_value_a));
    BOOST_CHECK(o_value_a && *o_value == *o_value_a);

    std::remove(testfile);
    return EXIT_SUCCESS;
}

#include <boost/serialization/optional.hpp>
#ifndef BOOST_NO_CXX17_HDR_OPTIONAL
#include <optional>
#endif

int test_main( int /* argc */, char* /* argv */[] ){
    test<boost::optional>();
    #ifndef BOOST_NO_CXX17_HDR_OPTIONAL
    test<std::optional>();
    #endif
    #ifdef BOOST_SERIALIZATION_TEST_OPTIONAL_MOVE_ONLY
    test_move_only<boost::optional>();
    #ifndef BOOST_NO_CXX17_HDR_OPTIONAL
    test_move_only<std::optional>();
    #endif
    #endif
    test_non_default_ctor<boost::optional>();
    #ifndef BOOST_NO_CXX17_HDR_OPTIONAL
    test_non_default_ctor<std::optional>();
    #endif
    return EXIT_SUCCESS;
}
