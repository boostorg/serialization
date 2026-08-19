/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_iterators_copy.cpp

// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// Regression test for issue #229.

#include <algorithm>
#include <cstdlib>
#include <cstddef>
#include <list>

#include <boost/config.hpp>

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include "test_tools.hpp"

// Traverse [it, end) but copy construct the iterator at every step and
// continue from the copy, the way std::copy unwraps an iterator.  The `== end`
// comparison is what makes transform_width set its end-of-sequence flag, so we
// must keep comparing against end for the copy to have that state to preserve.
template<class Iterator, class Output>
void drain_to_end_by_copy(Iterator it, Iterator end, Output out){
    while(! (it == end)){
        Iterator cur = it;   // copy constructor under test
        *out++ = *cur;
        ++cur;
        it = cur;
    }
}

// Read n elements, copy constructing the iterator at every step.  Used for the
// decode pipeline, which has no end-of-sequence padding but does drive
// remove_whitespace (whose copy must preserve its cached-value flag).
template<class Iterator, class Output>
void drain_n_by_copy(Iterator it, std::size_t n, Output out){
    for(std::size_t i = 0; i < n; ++i){
        Iterator cur = it;   // copy constructor under test
        *out++ = *cur;
        ++cur;
        it = cur;
    }
}

template<class CharType>
void test_base64_copy(unsigned int size){
    CharType rawdata[150];
    for(unsigned int i = 0; i < size; ++i)
        rawdata[i] = static_cast<CharType>(std::rand() & 0xff);

    typedef boost::archive::iterators::insert_linebreaks<
        boost::archive::iterators::base64_from_binary<
            boost::archive::iterators::transform_width<
                CharType *, 6, sizeof(CharType) * 8
            >
        >, 76
    > encode;

    // Straight encode (single iterator instance, as std::copy drives it).
    std::list<CharType> plain;
    std::copy(
        encode(rawdata), encode(rawdata + size), std::back_inserter(plain)
    );

    // Same encode, but copy constructing the iterator at every step.  Without
    // a correct transform_width copy constructor the final (zero padded)
    // group is produced from lost state, so the tails differ.
    std::list<CharType> copied;
    drain_to_end_by_copy(
        encode(rawdata), encode(rawdata + size), std::back_inserter(copied)
    );
    BOOST_CHECK(plain == copied);

    // Decode back to the original bytes, again copy constructing at every
    // step.  This drives remove_whitespace over the line breaks inserted
    // above (present once the base64 exceeds 76 characters).
    typedef boost::archive::iterators::transform_width<
        boost::archive::iterators::binary_from_base64<
            boost::archive::iterators::remove_whitespace<
                typename std::list<CharType>::iterator
            >
        >, sizeof(CharType) * 8, 6
    > decode;
    std::list<CharType> decoded;
    drain_n_by_copy(decode(plain.begin()), size, std::back_inserter(decoded));
    BOOST_CHECK(std::equal(rawdata, rawdata + size, decoded.begin()));
}

int test_main(int /* argc */, char * /* argv */ []){
    for(unsigned int s = 1; s <= 4; ++s)
        test_base64_copy<char>(s);
    test_base64_copy<char>(150);
    #ifndef BOOST_NO_CWCHAR
    for(unsigned int s = 1; s <= 4; ++s)
        test_base64_copy<wchar_t>(s);
    test_base64_copy<wchar_t>(150);
    #endif
    return EXIT_SUCCESS;
}
