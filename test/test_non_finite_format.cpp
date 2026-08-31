/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_non_finite_format.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// The library writes an infinity and a NaN as a special tag, so that an
// archive does not depend on which standard library produced it.

// Suggested by Robert Ramey in
// https://github.com/boostorg/serialization/pull/387.  Thanks!

#include <limits>
#include <sstream>
#include <string>

#include <boost/core/lightweight_test.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

template<class T>
static std::string written(T value){
    std::ostringstream os;
    {
        boost::archive::text_oarchive oa(os);
        oa << value;
    }
    const std::string all = os.str();
    // Everything after the last space of the header is the value itself.
    const std::string::size_type at = all.rfind(' ');
    BOOST_TEST(std::string::npos != at);
    std::string token = all.substr(at + 1);
    while(! token.empty()
    && ('\n' == token.back() || '\r' == token.back())){
        token.erase(token.size() - 1);
    }
    return token;
}

// Whatever the spelling, it has to come back as the same value.
template<class T>
static void reads_back_as(const std::string & token, T expected){
    // Take a real header, then put the spelling under test where the value
    // goes.  A zero is written as "0.00000000e+00", so the value cannot be
    // found by looking for a digit; the last space of the header is the mark.
    std::ostringstream os;
    {
        boost::archive::text_oarchive oa(os);
        const T zero = T(0);
        oa << zero;
    }
    std::string archive = os.str();
    const std::string::size_type at = archive.rfind(' ');
    BOOST_TEST(std::string::npos != at);
    archive.erase(at + 1);
    archive += token;
    archive += "\n";

    std::istringstream is(archive);
    boost::archive::text_iarchive ia(is);
    T back = T(1);
    ia >> back;
    if(expected == expected){
        BOOST_TEST(back == expected);
    }
    else{
        BOOST_TEST(back != back);    // a NaN was asked for
    }
}

template<class T>
static void test_type(){
    BOOST_TEST_EQ(written(std::numeric_limits<T>::infinity()), std::string("inf"));
    BOOST_TEST_EQ(written(-std::numeric_limits<T>::infinity()), std::string("-inf"));
    BOOST_TEST_EQ(written(std::numeric_limits<T>::quiet_NaN()), std::string("nan"));

    // Spellings older archives may carry are still accepted.
    reads_back_as<T>("inf", std::numeric_limits<T>::infinity());
    reads_back_as<T>("infinity", std::numeric_limits<T>::infinity());
    reads_back_as<T>("INF", std::numeric_limits<T>::infinity());
    reads_back_as<T>("-inf", -std::numeric_limits<T>::infinity());
    reads_back_as<T>("nan", std::numeric_limits<T>::quiet_NaN());
    reads_back_as<T>("nan(ind)", std::numeric_limits<T>::quiet_NaN());

    // What the Microsoft library wrote before 2015.  These begin with digits,
    // so the extraction takes the "1." for the value and succeeds; without
    // the rest being read they would load as 1 rather than fail outright.
    reads_back_as<T>("1.#INF", std::numeric_limits<T>::infinity());
    reads_back_as<T>("-1.#INF", -std::numeric_limits<T>::infinity());
    reads_back_as<T>("1.#QNAN", std::numeric_limits<T>::quiet_NaN());
    reads_back_as<T>("-1.#IND", std::numeric_limits<T>::quiet_NaN());
}

int
main(){
    test_type<float>();
    test_type<double>();
    return boost::report_errors();
}
