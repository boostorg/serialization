/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_iterators.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <iterator>
#include <list>
#include <string>

#if (defined _MSC_VER) && (_MSC_VER == 1200)
#  pragma warning (disable : 4786) // too long name, harmless warning
#endif

#include <cstdlib>
#include <cstddef> // size_t

#include <boost/config.hpp>
#ifdef BOOST_NO_STDC_NAMESPACE
namespace std{
    using ::rand;
    using ::size_t;
}
#endif

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include "test_tools.hpp"

#include <iostream>

template<typename CharType>
void test_base64(unsigned int size){
    CharType rawdata[150];
    CharType * rptr;
    for(rptr = rawdata + size; rptr-- > rawdata;)
        *rptr = static_cast<CharType>(std::rand()& 0xff);

    // convert to base64
    typedef std::list<CharType> text_base64_type;
    text_base64_type text_base64;

    typedef
        boost::archive::iterators::insert_linebreaks<
            boost::archive::iterators::base64_from_binary<
                boost::archive::iterators::transform_width<
                    CharType *
                    ,6
                    ,sizeof(CharType) * 8
                >
            >
            ,76
        >
        translate_out;

    std::copy(
        translate_out(static_cast<CharType *>(rawdata)),
        translate_out(rawdata + size),
        std::back_inserter(text_base64)
    );

    // convert from base64 to binary and compare with the original
    typedef
        boost::archive::iterators::transform_width<
            boost::archive::iterators::binary_from_base64<
                boost::archive::iterators::remove_whitespace<
                    typename text_base64_type::iterator
                >
            >,
            sizeof(CharType) * 8,
            6
        > translate_in;

    BOOST_CHECK(
        std::equal(
            rawdata,
            rawdata + size,
            translate_in(text_base64.begin())
        )
    );

}

// A base64 sequence whose length is not a whole number of four character
// groups carries some bits which do not make up a byte.  Decoding must drop
// them and stop, rather than look for the byte's remaining bits beyond the
// end of the input.  Reported by ROCKFAL1 in
// https://github.com/boostorg/serialization/issues/324.  Thanks!
void test_base64_partial_group(){
    typedef boost::archive::iterators::transform_width<
        boost::archive::iterators::binary_from_base64<const char *>, 8, 6
    > decoder;

    // "1234567890" encoded, less the padding
    const std::string encoded("MTIzNDU2Nzg5MA");
    const std::string decoded("1234567890");

    for(std::size_t n = 0; n <= encoded.size(); ++n){
        const char * const first = encoded.data();
        const char * const last = first + n;
        std::string result;
        std::copy(
            decoder(first, last),
            decoder(last),
            std::back_inserter(result)
        );
        // six bits in, eight bits out, and no partial byte at the end
        BOOST_CHECK(result == decoded.substr(0, n * 6 / 8));
    }
}

int
test_main( int /*argc*/, char* /*argv*/[] )
{
    test_base64_partial_group();
    test_base64<char>(1);
    test_base64<char>(2);
    test_base64<char>(3);
    test_base64<char>(4);
    test_base64<char>(150);
    #ifndef BOOST_NO_CWCHAR
    test_base64<wchar_t>(1);
    test_base64<wchar_t>(2);
    test_base64<wchar_t>(3);
    test_base64<wchar_t>(4);
    test_base64<wchar_t>(150);
    #endif
    return EXIT_SUCCESS;
}
