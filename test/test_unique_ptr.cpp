/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_unique_ptr.cpp

// (C) Copyright 2002-14 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#include <cstdio> // remove, std::autoptr inteface wrong in dinkumware
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif
#ifndef BOOST_NO_CXX11_SMART_PTR
#include <boost/serialization/unique_ptr.hpp>
#endif

#include <boost/serialization/nvp.hpp>

#include "test_tools.hpp"

/////////////////////////////////////////////////////////////
// test auto_ptr serialization
class A
{
private:
    friend class boost::serialization::access;
    int x;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & BOOST_SERIALIZATION_NVP(x);
    }
public:
    A(){}    // default constructor
    ~A(){}   // default destructor
};

void save(const std::unique_ptr<A> & spa, const char *filename)
{
    test_ostream os(filename, TEST_STREAM_FLAGS);
    test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
    oa << BOOST_SERIALIZATION_NVP(spa);
}

void load(std::unique_ptr<A> & spa, const char *filename)
{
    test_istream is(filename, TEST_STREAM_FLAGS);
    test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
    ia >> BOOST_SERIALIZATION_NVP(spa);
}

int test_main(int /* argc */, char * /* argv */[]){
    #ifndef BOOST_NO_CXX11_SMART_PTR
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    // create  a new auto pointer to ta new object of type A
    std::unique_ptr<A> spa(new A);
    // serialize it
    save(spa, testfile);
    // reset the auto pointer to NULL
    // thereby destroying the object of type A
    // note that the reset automagically maintains the reference count
    spa.reset();
    // restore state to one equivalent to the original
    // creating a new type A object
    load(spa, testfile);
    // obj of type A gets destroyed
    // as auto_ptr goes out of scope
    std::remove(testfile);
    #endif // BOOST_NO_CXX11_SMART_PTR
    return EXIT_SUCCESS;
}
