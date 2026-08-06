/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_string_allocator.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// A basic_string which does not use std::allocator, std::pmr::string being
// the usual case, has to round trip like std::string does and to produce
// the same representation, so that the two interoperate.

// Reported by schorsch1976 in
// https://github.com/boostorg/serialization/issues/267.  The report showed
// that declaring such a string primitive_type silently reads back only up
// to the first space, which is what led to the support added here.  Thanks
// for the clear reproducer!

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::remove;
}
#endif

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

#include "test_tools.hpp"

// The smallest allocator basic_string accepts, so that the string below
// differs from std::string in its allocator only.
template<class T>
struct custom_allocator
{
    typedef T value_type;
    custom_allocator(){}
    template<class U>
    custom_allocator(const custom_allocator<U> &){}
    T * allocate(std::size_t n){
        return std::allocator<T>().allocate(n);
    }
    void deallocate(T * p, std::size_t n){
        std::allocator<T>().deallocate(p, n);
    }
    template<class U>
    bool operator==(const custom_allocator<U> &) const {
        return true;
    }
    template<class U>
    bool operator!=(const custom_allocator<U> &) const {
        return false;
    }
};

typedef std::basic_string<
    char, std::char_traits<char>, custom_allocator<char>
> custom_string;

#ifndef BOOST_NO_STD_WSTRING
typedef std::basic_string<
    wchar_t, std::char_traits<wchar_t>, custom_allocator<wchar_t>
> custom_wstring;
#endif

template<class String>
struct data
{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & BOOST_SERIALIZATION_NVP(value);
        ar & BOOST_SERIALIZATION_NVP(tail);
    }
    String value;
    int tail;
};

template<class String>
static void save(const char * testfile, const char * text){
    data<String> d;
    d.value = text;
    d.tail = 42;
    test_ostream os(testfile, TEST_STREAM_FLAGS);
    test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
    oa << BOOST_SERIALIZATION_NVP(d);
}

template<class String>
static std::string load(const char * testfile){
    data<String> d;
    d.tail = 0;
    test_istream is(testfile, TEST_STREAM_FLAGS);
    test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
    ia >> BOOST_SERIALIZATION_NVP(d);
    // the member after the string has to survive too: reading a string
    // past its end used to leave the rest of the archive unusable
    BOOST_CHECK(42 == d.tail);
    return std::string(d.value.begin(), d.value.end());
}

// text which the old primitive_type route mishandled: a space stopped the
// token read, and the xml tag characters confused it further
static const char * const texts[] = {
    "D1",
    "a b",
    "",
    "  lead and trail ",
    "tag </value> and & < > chars"
};

static void test_roundtrip(const char * testfile, const char * text){
    save<custom_string>(testfile, text);
    BOOST_CHECK(load<custom_string>(testfile) == std::string(text));
}

// the representation has to match std::string's, so that an archive written
// with one loads into the other
static void test_interoperates(const char * testfile, const char * text){
    save<std::string>(testfile, text);
    BOOST_CHECK(load<custom_string>(testfile) == std::string(text));

    save<custom_string>(testfile, text);
    BOOST_CHECK(load<std::string>(testfile) == std::string(text));
}

#ifndef BOOST_NO_STD_WSTRING
// the wide string borrows the std::wstring primitive the same way
static void test_wide(const char * testfile, const wchar_t * text){
    {
        data<custom_wstring> d;
        d.value = text;
        d.tail = 42;
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << BOOST_SERIALIZATION_NVP(d);
    }
    data<custom_wstring> d;
    d.tail = 0;
    test_istream is(testfile, TEST_STREAM_FLAGS);
    test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
    ia >> BOOST_SERIALIZATION_NVP(d);
    BOOST_CHECK(42 == d.tail);
    BOOST_CHECK(std::wstring(d.value.begin(), d.value.end())
        == std::wstring(text));
}
#endif

int
test_main(int /* argc */, char * /* argv */ [])
{
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    const std::size_t n = sizeof(texts) / sizeof(texts[0]);
    for(std::size_t i = 0; i < n; ++i){
        test_roundtrip(testfile, texts[i]);
        test_interoperates(testfile, texts[i]);
    }

    #ifndef BOOST_NO_STD_WSTRING
    test_wide(testfile, L"D1");
    test_wide(testfile, L"a b");
    test_wide(testfile, L"");
    #endif

    std::remove(testfile);
    return EXIT_SUCCESS;
}
