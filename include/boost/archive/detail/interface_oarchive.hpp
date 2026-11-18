#ifndef BOOST_ARCHIVE_DETAIL_INTERFACE_OARCHIVE_HPP
#define BOOST_ARCHIVE_DETAIL_INTERFACE_OARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// interface_oarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
#include <cstddef> // NULL
#include <boost/cstdint.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/detail/workaround.hpp>

#include <boost/archive/detail/auto_link_archive.hpp>
#include <boost/archive/detail/oserializer.hpp>
#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

#include <boost/serialization/singleton.hpp>

namespace boost {
namespace archive {
namespace detail {

class basic_pointer_oserializer;

template<class Archive>
class interface_oarchive
{
protected:
    #if defined(_MSC_VER)
    // MSVC 14.52, unlike earlier versions, converts a pointer to an empty
    // base which ends its derived class with an offset one less than the
    // layout says, so a static library and a program built by different
    // versions disagree on where the archive is.  This member keeps the
    // base from being empty.  Microsoft has fixed the bug for a later
    // release, so the member can be removed once 14.52 is no longer in
    // use; see
    // <https://developercommunity.visualstudio.com/t/C-object-layout-places-empty-non-first/11175160>
    // and <https://github.com/boostorg/serialization/issues/389>.
    //
    // Note that we test _MSC_VER, not BOOST_MSVC: clang-cl and the Intel
    // compiler define it too, and as they use the same layout as MSVC and
    // link with libraries built by it, they must agree on the member.
    char m_msvc_14_52_workaround;
    #endif
    interface_oarchive() {}
public:
    /////////////////////////////////////////////////////////
    // archive public interface
    typedef mpl::bool_<false> is_loading;
    typedef mpl::bool_<true> is_saving;

    // return a pointer to the most derived class
    Archive * This(){
        return static_cast<Archive*>(this);
    }

    template<class T>
    const basic_pointer_oserializer *
    register_type(const T * = NULL){
        const basic_pointer_oserializer & bpos =
            boost::serialization::singleton<
                pointer_oserializer<Archive, T>
            >::get_const_instance();
        this->This()->register_basic_serializer(bpos.get_basic_serializer());
        return & bpos;
    }

    template<class Helper>
    Helper &
    get_helper(void * const id = 0){
        helper_collection & hc = this->This()->get_helper_collection();
        return hc.template find_helper<Helper>(id);
    }

    template<class T>
    Archive & operator<<(const T & t){
        this->This()->save_override(t);
        return * this->This();
    }

    // the & operator
    template<class T>
    Archive & operator&(const T & t){
        return * this ->This() << t;
    }
};

} // namespace detail
} // namespace archive
} // namespace boost

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // BOOST_ARCHIVE_DETAIL_INTERFACE_IARCHIVE_HPP
