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
#include <map>

#include <boost/shared_ptr.hpp>

namespace boost {
namespace archive {
namespace detail {

class helper_collection
{
    helper_collection(const helper_collection&);              // non-copyable
    helper_collection& operator = (const helper_collection&); // non-copyable

    typedef std::map<
        const boost::serialization::extended_type_info *,
        shared_ptr<void> >  collection;
    typedef collection::value_type helper_value_type;
    typedef collection::iterator helper_iterator;

    // dynamically allocated to minimize penalty when not used
    collection * m_helpers; 

    collection& helpers()
    {
        if(!m_helpers) m_helpers = new collection;
        return *m_helpers;
    };
protected:
    helper_collection(): m_helpers(NULL){}
    ~helper_collection()
    {
        delete m_helpers;
    }
public:
    template<typename Helper>
    Helper& get_helper(Helper * = NULL)
    {
        const boost::serialization::extended_type_info * eti =
            &boost::serialization::type_info_implementation<Helper>::
                type::get_const_instance();
        helper_iterator it = helpers().find(eti);
        if(it == helpers().end())
          it = helpers().insert(
              helper_value_type(
                  eti,
                  shared_ptr<void>(new Helper))).first;
        return *static_cast<Helper *>(it->second.get());
    }
};

// conditionally derive from helper_collection
template<bool Derive>
struct derive_from_helper_collection{};

template<>
struct derive_from_helper_collection<true>: helper_collection{};

} // namespace detail
} // namespace serialization
} // namespace boost

#endif // BOOST_ARCHIVE_DETAIL_HELPER_COLLECTION_HPP
