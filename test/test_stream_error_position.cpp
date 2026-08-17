/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_stream_error_position.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// An input_stream_error used to say only that the input had failed.  The
// exception now names the offset the input went wrong at.

// Suggested by swebb2066 in
// https://github.com/boostorg/serialization/pull/318.  Thanks!

// The offset itself is not checked against a fixed number: it moves with the
// archive version, and with the width of whatever the header happens to hold.
// What has to hold is that an offset is reported at all, that it is a number,
// and that it falls inside the archive.

#include <cstddef>
#include <cstdlib> // atol
#include <sstream>
#include <string>

// The lightweight test of Boost.Core is used here in place of
// test_tools.hpp, whose test_main runs one archive type per build.  Every
// archive type has to be exercised in the same run, because each reaches the
// offset by a different route: the text primitive through a failed
// extraction, the binary one through a short read, and the XML one through a
// parse which ran out of input.
#include <boost/core/lightweight_test.hpp>

#include <boost/archive/archive_exception.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>

struct pair_of_ints {
    int x;
    int y;
    pair_of_ints() : x(0), y(0) {}
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */){
        ar & BOOST_SERIALIZATION_NVP(x);
        ar & BOOST_SERIALIZATION_NVP(y);
    }
};

// Returns the offset named by the message, or -1 if it names none.  A
// message reads "input stream error-at offset 36".
static long reported_offset(const std::string & message){
    const std::string tag("at offset ");
    const std::size_t at = message.find(tag);
    if(std::string::npos == at){
        return -1;
    }
    const std::string digits = message.substr(at + tag.size());
    if(digits.empty() || '0' > digits[0] || '9' < digits[0]){
        return -1;
    }
    return std::atol(digits.c_str());
}

template<class OArchive>
static std::string archive_of_a_pair(){
    std::ostringstream os;
    {
        OArchive oa(os);
        pair_of_ints p;
        p.x = 11;
        p.y = 22;
        oa << BOOST_SERIALIZATION_NVP(p);
    }
    return os.str();
}

// Loads the given text as an archive and returns the offset the resulting
// input_stream_error names, or -1 if the load did not fail that way.
template<class IArchive>
static long offset_of_failure(const std::string & data){
    BOOST_TRY {
        std::istringstream is(data);
        IArchive ia(is);
        pair_of_ints p;
        ia >> BOOST_SERIALIZATION_NVP(p);
    }
    BOOST_CATCH(const boost::archive::archive_exception & e){
        if(boost::archive::archive_exception::input_stream_error != e.code){
            return -1;
        }
        return reported_offset(e.what());
    }
    BOOST_CATCH_END
    return -1;
}

// A value the archive cannot read back, put where the second int was
// written.  The text and binary archives keep the last "22" for that int;
// the XML one writes it before the closing tags, so the first occurrence is
// the one to spoil.
template<class OArchive, class IArchive>
static void test_corrupted_value(bool from_front){
    std::string data = archive_of_a_pair<OArchive>();
    const std::size_t at = from_front ? data.find("22") : data.rfind("22");
    BOOST_TEST(std::string::npos != at);
    data.replace(at, 2, "zz");

    const long offset = offset_of_failure<IArchive>(data);
    BOOST_TEST_GT(offset, 0);
    BOOST_TEST_LE(offset, static_cast<long>(data.size()));
}

// An archive cut short, so that the failure comes from running out of input
// rather than from a value which will not parse.
template<class OArchive, class IArchive>
static void test_truncated(std::size_t chop){
    std::string data = archive_of_a_pair<OArchive>();
    BOOST_TEST_GT(data.size(), chop);
    data.resize(data.size() - chop);

    const long offset = offset_of_failure<IArchive>(data);
    BOOST_TEST_GT(offset, 0);
    BOOST_TEST_LE(offset, static_cast<long>(data.size()));
}

int
main(){
    using namespace boost::archive;

    test_corrupted_value<text_oarchive, text_iarchive>(false);
    test_corrupted_value<xml_oarchive, xml_iarchive>(true);

    test_truncated<binary_oarchive, binary_iarchive>(6);
    test_truncated<xml_oarchive, xml_iarchive>(30);

    return boost::report_errors();
}
