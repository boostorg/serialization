/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_dll_base_pointer.cpp

// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// Regression test for issue #117.  A derived class defined in a shared
// library is serialized and deserialized through a pointer to its abstract
// base, from an executable built with hidden symbol visibility.
// Historically this failed on GCC/ELF: the per type serializer singletons
// were duplicated across the shared library boundary, so the reader could not
// map the class id back to the derived type.  The round trip must recover an
// object of the derived type.

#include <fstream>
#include <cstdio>
#include <typeinfo>

#include <boost/config.hpp>

#include "test_tools.hpp"

#ifndef BOOST_NO_CXX11_SMART_PTR

#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
#include <boost/archive/tmpdir.hpp>

#include <boost/smart_ptr/make_unique.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/nvp.hpp>

// polymorphic_derived2 (and its base, polymorphic_base) live in the shared
// library this test links against
#define POLYMORPHIC_DERIVED2_IMPORT
#include "polymorphic_derived2.hpp"

int test_main(int /* argc */, char * /* argv */ []){
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    std::unique_ptr<polymorphic_base> saved =
        boost::make_unique<polymorphic_derived2>();
    {
        std::ofstream os(testfile, std::ios::binary);
        boost::archive::polymorphic_binary_oarchive oa(os);
        boost::archive::polymorphic_oarchive & poa = oa;
        poa << boost::serialization::make_nvp("ptr", saved);
    }

    std::unique_ptr<polymorphic_base> loaded;
    {
        std::ifstream is(testfile, std::ios::binary);
        boost::archive::polymorphic_binary_iarchive ia(is);
        boost::archive::polymorphic_iarchive & pia = ia;
        pia >> boost::serialization::make_nvp("ptr", loaded);
    }

    BOOST_CHECK(0 != loaded.get());
    // the object recovered through the base pointer must be the derived type
    BOOST_CHECK(0 != dynamic_cast<polymorphic_derived2 *>(loaded.get()));

    std::remove(testfile);
    return EXIT_SUCCESS;
}

#else

int test_main(int /* argc */, char * /* argv */ []){
    return EXIT_SUCCESS;
}

#endif // BOOST_NO_CXX11_SMART_PTR
