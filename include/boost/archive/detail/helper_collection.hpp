#ifndef BOOST_ARCHIVE_DETAIL_HELPER_COLLECTION_HPP
#define BOOST_ARCHIVE_DETAIL_HELPER_COLLECTION_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// helper_collection.hpp: archive support for run-time helpers

// (C) Copyright 2002-2008 Robert Ramey and Joaquin M Lopez Munoz
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // NULL
#include <typeinfo>
#include <map>
#include <utility>

#include <boost/config.hpp>

#ifndef BOOST_NO_CXX11_SMART_PTR
    #include <memory>
#else
    #include <boost/serialization/type_info_implementation.hpp>
    #include <boost/smart_ptr/shared_ptr.hpp>
#endif

namespace boost {

namespace archive {
namespace detail {

class helper_collection
{
    helper_collection(const helper_collection&);              // non-copyable
    helper_collection& operator = (const helper_collection&); // non-copyable

    // note: we dont' actually "share" the function object pointer
    // we only use shared_ptr to make sure that it get's deleted

    // note: consider replacement of map with vector of pairs.  In practice
    // there will likely be only a couple of helper instances per
    // archive instance so map is way overkill in terms of time and space.

    #ifndef BOOST_NO_CXX11_SMART_PTR
        typedef std::map<
            const std::type_info *,
           std::shared_ptr<void>
        >  collection;
    #else
    #endif

    typedef collection::value_type helper_value_type;
    typedef collection::iterator helper_iterator;

    // dynamically allocated to minimize penalty when not used
    std::unique_ptr<collection> m_helpers;
 
    collection & helpers(){
        if(!m_helpers)
            m_helpers = std::unique_ptr<collection>(new collection);
        return * m_helpers;
    };
protected:
    helper_collection()
    {}
    ~helper_collection(){
    }
public:
    template<typename Helper>
    Helper& get_helper(Helper * = NULL) {
        const std::type_info * eti = & typeid(Helper);
        helper_iterator it = helpers().find(eti);
        if(it == helpers().end()){
            it = helpers().insert(
                std::make_pair(
                    eti,
                    std::make_shared<Helper>()
                )
            ).first;
        }
        return *static_cast<Helper *>(it->second.get());
    }
};

} // namespace detail
} // namespace serialization
} // namespace boost

#endif // BOOST_ARCHIVE_DETAIL_HELPER_COLLECTION_HPP
