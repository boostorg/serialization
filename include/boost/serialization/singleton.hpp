#ifndef BOOST_SERIALIZATION_SINGLETON_HPP
#define BOOST_SERIALIZATION_SINGLETON_HPP

/////////1/////////2///////// 3/////////4/////////5/////////6/////////7/////////8
//  singleton.hpp
//
// Copyright David Abrahams 2006. Original version
//
// Copyright Robert Ramey 2007.  Changes made to permit
// application throughout the serialization library.
//
// Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// The intention here is to define a template which will convert
// any class into a singleton with the following features:
//
// a) initialized before first use.
// b) thread-safe for const access to the class
// c) non-locking
//
// In order to do this,
// a) Initialize dynamically when used.
// b) Require that all singletons be initialized before main
// is called*.  This guarentees no race condition for initialization.
// In debug mode, assert that no non-const functions are called
// after main is invoked.
//
// * note exception regarding dynamically loaded shared libraries.
// check documentation

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif 

#include <cassert>
#include <boost/noncopyable.hpp>
#include <boost/serialization/force_include.hpp>

namespace boost { 
namespace serialization { 

//////////////////////////////////////////////////////////////////////
// Provides a dynamically-initialized (singleton) instance of T in a
// way that avoids LNK1179 on vc6.  See http://tinyurl.com/ljdp8 or
// http://lists.boost.org/Archives/boost/2006/05/105286.php for
// details.
//

template <class T>
class singleton : public boost::noncopyable
{
    friend class global_lock;
private:
    BOOST_DLLEXPORT static T & instance;
    // include this to provoke instantiation at pre-execution time
    static void use(T const &) {}
    static T & get_instance(){
        static T t;
        // refer to instance, causing it to be instantiated (and
        // initialized at startup on working compilers)
        use(instance);
        return t;
    }
public:
    static const T & get_const_instance();
    static T & get_mutable_instance();
};

template<class T>
BOOST_DLLEXPORT T & singleton<T>::instance = singleton<T>::get_instance();

// make a singleton to lock/unlock all singletons for alteration.
// The intent is that all singletons created/used by this code
// are to be initialized before main is called. (note exception
// for DLLS which is dealt with in the documentation).  If
// the singleton is then used only as read

class global_lock : public singleton<global_lock> {
    bool locked;
public:
    global_lock() : locked(false) {}
    void lock(){
        locked = true;
    }
    void unlock(){
        locked = false;
    }
    bool is_locked() const {
        return locked;
    }
    static global_lock & get_mutable_instance(){
        return get_instance();
    }
};

template<class T>
inline T & singleton<T>::get_mutable_instance(){
    assert(! global_lock::get_mutable_instance().is_locked());
    return get_instance();
}

template<class T>
inline const T & singleton<T>::get_const_instance(){
    return get_instance();
}


} // namespace serialization
} // namespace boost

#endif // BOOST_SERIALIZATION_SINGLETON_HPP
