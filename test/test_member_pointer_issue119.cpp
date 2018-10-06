/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_member_pointer_issue119.cpp

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//


#include <sstream>
#include <cstdlib>

#include <boost/config.hpp>
#include "test_tools.hpp"

#include <boost/archive/text_oarchive.hpp>

/*
 * This is to trigger a null pointer dereference in oserializer.hpp
 * which was fixed 2017-09-20.
 * The code is a minimized and simplifed version of the code example
 * by Simon Giraudoty, see https://github.com/boostorg/serialization/issues/119
 * which triggers the error in earlier versions of boost (<1.66), at least
 * if building with clang 6 with -O3 and definitly if using undefined behaviour
 * sanitizer.
 */

struct Node
{
  Node* ptr=nullptr;

  template <typename Archive>
  void serialize(Archive& ar, unsigned /* version */)
  {
    ar & ptr;
  }
};

int
test_main( int /* argc */, char* /* argv */[] )
{
    std::ostringstream oss;

    boost::archive::text_oarchive oa { oss };
    oa << Node();

    return EXIT_SUCCESS;
}
