// (C) Copyright 2002-4 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org for updates, documentation, and revision history.
// file includes for testing a custom archive.
// as an example this tests the portable binary archive

#include <fstream>

// #include output archive header
#include <boost/archive/portable_oarchive.hpp>
// define output archive class to be used 
typedef eos::portable_oarchive test_oarchive;
// and corresponding stream
typedef std::ofstream test_ostream;

// repeat the above for correspondng input archive
#include <boost/archive/portable_iarchive.hpp>
typedef eos::portable_iarchive test_iarchive;
typedef std::ifstream test_istream;

// and stream open flags
#define TEST_STREAM_FLAGS std::ios::binary
