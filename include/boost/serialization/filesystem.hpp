#ifndef BOOST_SERIALIZATION_FILESYSTEM_HPP
#define BOOST_SERIALIZATION_FILESYSTEM_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// filesystem.hpp - non-intrusive serialization of std::filesystem::path

// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>

#if defined(__has_include)
#  if __has_include(<filesystem>) && (BOOST_CXX_VERSION >= 201703L)
#    define BOOST_SERIALIZATION_HAS_STD_FILESYSTEM
#  endif
#endif

#ifdef BOOST_SERIALIZATION_HAS_STD_FILESYSTEM

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>

#include <string>
#include <filesystem>

namespace boost {
namespace serialization {

// The path is serialized as its UTF-8 representation rather than the native
// string.  This keeps archives portable between platforms (the native
// encoding is narrow on POSIX but wide on Windows) and never throws for
// characters the active narrow code page cannot represent.

template<class Archive>
void save(
    Archive & ar,
    const std::filesystem::path & p,
    const unsigned int /* version */
){
#ifdef __cpp_char8_t
    const std::u8string u8 = p.u8string();
    const std::string str(u8.cbegin(), u8.cend());
#else
    const std::string str = p.u8string();
#endif
    ar << boost::serialization::make_nvp("path", str);
}

template<class Archive>
void load(
    Archive & ar,
    std::filesystem::path & p,
    const unsigned int /* version */
){
    std::string str;
    ar >> boost::serialization::make_nvp("path", str);
#ifdef __cpp_char8_t
    p = std::filesystem::path(std::u8string(str.cbegin(), str.cend()));
#else
    p = std::filesystem::u8path(str);
#endif
}

template<class Archive>
void serialize(
    Archive & ar,
    std::filesystem::path & p,
    const unsigned int version
){
    boost::serialization::split_free(ar, p, version);
}

} // namespace serialization
} // namespace boost

#endif // BOOST_SERIALIZATION_HAS_STD_FILESYSTEM

#endif // BOOST_SERIALIZATION_FILESYSTEM_HPP
