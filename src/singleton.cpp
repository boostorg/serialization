/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// singleton.cpp
//
// Copyright (c) 201 5 Robert Ramey, Indiana University (garcia@osl.iu.edu)
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/serialization/config.hpp>
#include <boost/serialization/singleton.hpp>

namespace boost { 
namespace serialization { 

BOOST_SERIALIZATION_DECL bool & singleton_module::get_lock(){
    static bool lock = false;
    return lock;
}

} // namespace serialization
} // namespace boost
