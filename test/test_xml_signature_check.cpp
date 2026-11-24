/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_xml_signature_check.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// An XML input archive must accept only the exact signature in the header.
// The check used to compare just as many characters as the archive gave, so
// it accepted a prefix of the signature, and read past the end of it for a
// longer one.

// Reported by jhtrace in
// https://github.com/boostorg/serialization/issues/392.  Thanks!

#include <sstream>
#include <string>

#include <boost/archive/archive_exception.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "test_tools.hpp"

// The header of a valid archive, with the signature replaced.
std::string header_with_signature(const std::string & signature){
    std::ostringstream os;
    {
        boost::archive::xml_oarchive oa(os);
    }
    std::string header = os.str();
    const std::string attribute = "signature=\"";
    const std::size_t first = header.find(attribute) + attribute.size();
    const std::size_t last = header.find('"', first);
    header.replace(first, last - first, signature);
    return header;
}

bool is_accepted(const std::string & signature){
    bool accepted = true;
    std::istringstream is(header_with_signature(signature));
    try{
        boost::archive::xml_iarchive ia(is);
    }
    catch(const boost::archive::archive_exception & e){
        BOOST_CHECK(
            e.code == boost::archive::archive_exception::invalid_signature
        );
        accepted = false;
    }
    return accepted;
}

int test_main(int /* argc */, char * /* argv */ []){
    BOOST_CHECK(is_accepted("serialization::archive"));
    BOOST_CHECK(! is_accepted("serialization::archive_AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
    BOOST_CHECK(! is_accepted("serialization"));
    return EXIT_SUCCESS;
}
