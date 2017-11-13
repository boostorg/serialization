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
// is called or any entry point into the shared library is invoked.
// This guarentees no race condition for initialization.
// In debug mode, we assert that no non-const functions are called
// after main is invoked.
//

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif 

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/noncopyable.hpp>
#include <boost/serialization/force_include.hpp>

#include <boost/archive/detail/auto_link_archive.hpp>
#include <boost/serialization/config.hpp>
#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace boost { 
namespace serialization { 

//////////////////////////////////////////////////////////////////////
// Provides a dynamically-initialized (singleton) instance of T in a
// way that avoids LNK1179 on vc6.  See http://tinyurl.com/ljdp8 or
// http://lists.boost.org/Archives/boost/2006/05/105286.php for
// details.
//

// singletons created by this code are guarenteed to be unique
// within the executable or shared library which creates them.
// This is sufficient and in fact ideal for the serialization library.
// The singleton is created when the module is loaded and destroyed
// when the module is unloaded.

// This base class has two functions.

// First it provides a module handle for each singleton indicating
// the executable or shared library in which it was created. This
// turns out to be necessary and sufficient to implement the tables
// used by serialization library.

// Second, it provides a mechanism to detect when a non-const function
// is called after initialization.

// make a singleton to lock/unlock all singletons for alteration.
// The intent is that all singletons created/used by this code
// are to be initialized before main is called. A test program
// can lock all the singletons when main is entereed.  This any
// attempt to retieve a mutable instances while locked will
// generate a assertion if compiled for debug.

class BOOST_SERIALIZATION_DECL singleton_module :
    public boost::noncopyable
{
private:
static bool & get_lock() BOOST_USED;
public:
    static void lock(){
        get_lock() = true;
    }
    static void unlock(){
        get_lock() = false;
    }
    static bool is_locked(){
        return get_lock();
    }
};

template <class T>
class singleton : public singleton_module
{
private:
    static T & m_instance;
    // include this to provoke instantiation at pre-execution time
    static void use(T const *) {}
    static T & get_instance() {
        // use a wrapper so that types T with protected constructors
        // can be used
        class singleton_wrapper : public T {};
        static singleton_wrapper t;
        // refer to instance, causing it to be instantiated (and
        // initialized at startup on working compilers)
        BOOST_ASSERT(! is_destroyed());
        // note that the following is absolutely essential.
        // Without this, compilers/linkers will strip out "dead code"
        // as it looks like it is never referred to.  But it is.
        // commenting out this statement will cause compilers to fail to
        // construct the instance at pre-execution time.  This would prevent
        // our usage/implementation of "locking" and introduce uncertainty into
        // the sequence of object initializaition.
        use(& m_instance);
        return static_cast<T &>(t);
    }
    static bool & get_is_destroyed(){
        static bool is_destroyed;
        return is_destroyed;
    }

public:
    static T & get_mutable_instance(){
        BOOST_ASSERT(! is_locked());
        return get_instance();
    }
    static const T & get_const_instance(){
        return get_instance();
    }
    static bool is_destroyed(){
        return get_is_destroyed();
    }
    singleton(){
        get_is_destroyed() = false;
        unlock();
    }
    ~singleton() {
        get_is_destroyed() = true;
    }
};

template<class T>
T & singleton< T >::m_instance = singleton< T >::get_instance();

} // namespace serialization
} // namespace boost

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#endif // BOOST_SERIALIZATION_SINGLETON_HPP
