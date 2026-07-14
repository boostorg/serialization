/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_filesystem_path.cpp

// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <string>

#include <boost/config.hpp>

// `std::filesystem::path` is a C++17 feature; when it isn't available, this
// test has nothing to exercise and simply succeeds.
#if defined(__has_include)
#  if __has_include(<filesystem>) && (BOOST_CXX_VERSION >= 201703L)
#    define BOOST_SERIALIZATION_TEST_STD_FILESYSTEM
#  endif
#endif

#include "test_tools.hpp"

#ifdef BOOST_SERIALIZATION_TEST_STD_FILESYSTEM

#include <boost/serialization/filesystem.hpp>
#include <filesystem>

void check_roundtrip(const std::filesystem::path & original){
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);
    {
        test_ostream os(testfile, TEST_STREAM_FLAGS);
        test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
        oa << boost::serialization::make_nvp("path", original);
    }
    std::filesystem::path restored;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        ia >> boost::serialization::make_nvp("path", restored);
    }
    BOOST_CHECK(original == restored);
    std::remove(testfile);
}

// Build a path from a UTF-8 byte string, portably across C++17 and C++20.
std::filesystem::path from_utf8(const std::string & utf8){
#ifdef __cpp_char8_t
    return std::filesystem::path(std::u8string(utf8.begin(), utf8.end()));
#else
    return std::filesystem::u8path(utf8);
#endif
}

int test_main(int /* argc */, char * /* argv */ []){
    check_roundtrip(std::filesystem::path());              // empty
    check_roundtrip("foo/bar/baz.txt");                    // relative
    check_roundtrip("/absolute/unix/style/path");          // absolute-ish
    check_roundtrip("with spaces/and.dots/file.ext");
    // A path with non-ASCII components, to exercise the UTF-8 round trip.
    // The escapes are the UTF-8 encoding of U+00E9, U+00EF and U+00FC
    // (e-acute, i-diaeresis, u-diaeresis); written as `\x` so the source
    // file stays pure ASCII and encoding-independent.
    check_roundtrip(from_utf8("caf\xC3\xA9/na\xC3\xAF" "ve/\xC3\xBC.txt"));
    return EXIT_SUCCESS;
}

#else // std::filesystem::path unavailable

int test_main(int /* argc */, char * /* argv */ []){
    return EXIT_SUCCESS;
}

#endif
