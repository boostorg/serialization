#ifndef BOOST_ARCHIVE_BINARY_WIARCHIVE_HPP
#define BOOST_ARCHIVE_BINARY_WIARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// binary_wiarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>
#ifdef BOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#else

#include <istream> // wistream
#include <boost/archive/binary_iarchive_impl.hpp>

namespace boost { 
namespace archive {

// same as binary_wiarchive below - without helper support
class naked_binary_wiarchive : 
    public binary_iarchive_impl<
        boost::archive::naked_binary_wiarchive, 
        std::wistream::char_type, 
        std::wistream::traits_type,
        false // no helper support
    >
{
public:
    naked_binary_wiarchive(std::wistream & is, unsigned int flags = 0) :
        binary_iarchive_impl<
            naked_binary_wiarchive, 
            std::wistream::char_type, 
            std::wistream::traits_type,
            false
        >(is, flags)
    {}
    naked_binary_wiarchive(std::wstreambuf & bsb, unsigned int flags = 0) :
        binary_iarchive_impl<
            naked_binary_wiarchive, 
            std::wistream::char_type, 
            std::wistream::traits_type,
            false
        >(bsb, flags)
    {}
};

class binary_wiarchive : 
    public binary_iarchive_impl<
        binary_wiarchive,
        std::wistream::char_type,
        std::wistream::traits_type,
        true // helper support
    >
{
public:
    binary_wiarchive(std::wistream & is, unsigned int flags = 0) :
        binary_iarchive_impl<
            binary_wiarchive,
            std::wistream::char_type,
            std::wistream::traits_type,
            true
        >(is, flags)
    {}
    binary_wiarchive(std::wstreambuf & bsb, unsigned int flags = 0) :
        binary_iarchive_impl<
            binary_wiarchive,
            std::wistream::char_type,
            std::wistream::traits_type,
            true
        >(bsb, flags)
    {}
};

} // namespace archive
} // namespace boost

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::binary_wiarchive)

#endif // BOOST_NO_STD_WSTREAMBUF
#endif // BOOST_ARCHIVE_BINARY_WIARCHIVE_HPP
