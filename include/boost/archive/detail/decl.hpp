#ifndef BOOST_ARCHIVE_DETAIL_DECL_HPP
#define BOOST_ARCHIVE_DETAIL_DECL_HPP 

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif 

/////////1/////////2///////// 3/////////4/////////5/////////6/////////7/////////8
//  decl.hpp
//
//  (c) Copyright Robert Ramey 2004
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/serialization

//----------------------------------------------------------------------------// 

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

#include <boost/config.hpp>

#if defined(BOOST_HAS_DECLSPEC)
    #if (defined(BOOST_ALL_DYN_LINK) || defined(BOOST_SERIALIZATION_DYN_LINK))
        #if defined(BOOST_ARCHIVE_SOURCE)
            #define BOOST_ARCHIVE_DECL BOOST_SYMBOL_EXPORT
            #define BOOST_ARCHIVE_OR_WARCHIVE_DECL  BOOST_SYMBOL_EXPORT
            #define BOOST_ARCHIVE_SYMBOL_VISIBLE BOOST_SYMBOL_EXPORT
        #else
            #define BOOST_ARCHIVE_DECL BOOST_SYMBOL_IMPORT
        #endif
        #if defined(BOOST_WARCHIVE_SOURCE)
            #define BOOST_WARCHIVE_DECL BOOST_SYMBOL_EXPORT
            #define BOOST_ARCHIVE_OR_WARCHIVE_DECL BOOST_SYMBOL_EXPORT
        #else
            #define BOOST_WARCHIVE_DECL BOOST_SYMBOL_IMPORT
        #endif
        #if !defined(BOOST_WARCHIVE_SOURCE) && !defined(BOOST_ARCHIVE_SOURCE)
            #define BOOST_ARCHIVE_OR_WARCHIVE_DECL BOOST_SYMBOL_IMPORT
        #endif
    #endif
#endif // BOOST_HAS_DECLSPEC

#if ! defined(BOOST_ARCHIVE_DECL)
    #define BOOST_ARCHIVE_DECL
#endif
#if ! defined(BOOST_WARCHIVE_DECL)
    #define BOOST_WARCHIVE_DECL
#endif
#if ! defined(BOOST_ARCHIVE_OR_WARCHIVE_DECL)
    #define BOOST_ARCHIVE_OR_WARCHIVE_DECL
#endif

// All Win32 development environments, including 64-bit Windows and MinGW, define
// _WIN32 or one of its variant spellings. Note that Cygwin is a POSIX environment,
// so does not define _WIN32 or its variants.
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32)) && !defined(__CYGWIN__)
    #define BOOST_ARCHIVE_SYMBOL_VISIBLE BOOST_ARCHIVE_DECL
    #define BOOST_WARCHIVE_SYMBOL_VISIBLE BOOST_WARCHIVE_DECL
    #define BOOST_ARCHIVE_OR_WARCHIVE_SYMBOL_VISIBLE BOOST_ARCHIVE_OR_WARCHIVE_DECL
#else
    #define BOOST_ARCHIVE_SYMBOL_VISIBLE BOOST_SYMBOL_VISIBLE
    #define BOOST_WARCHIVE_SYMBOL_VISIBLE BOOST_SYMBOL_VISIBLE
    #define BOOST_ARCHIVE_OR_WARCHIVE_SYMBOL_VISIBLE BOOST_SYMBOL_VISIBLE
#endif

#endif // BOOST_ARCHIVE_DETAIL_DECL_HPP
