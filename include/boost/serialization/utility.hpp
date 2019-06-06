#ifndef  BOOST_SERIALIZATION_UTILITY_HPP
#define BOOST_SERIALIZATION_UTILITY_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// serialization/utility.hpp:
// serialization for stl utility templates

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <utility>
#include <boost/config.hpp>

#include <boost/core/addressof.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/is_bitwise_serializable.hpp>
#include <boost/serialization/detail/stack_constructor.hpp>
#include <boost/move/move.hpp>
#include <boost/mpl/and.hpp>

namespace boost {
namespace serialization {

// pair
template<class Archive, class F, class S>
inline void serialize(
    Archive &,
    std::pair<F, S> & ,
    const unsigned int /* file_version */
) {}

template <class Archive, class F, class S>
void save_construct_data(Archive & ar, const std::pair<F, S> * p,
                         const unsigned int file_version) {
  save_construct_data_adl(
      ar, ::boost::addressof(p->first), file_version);
  ar << boost::serialization::make_nvp("first", p->first);

  save_construct_data_adl(
      ar, ::boost::addressof(p->second), file_version);
  ar << boost::serialization::make_nvp("second", p->second);
}

template <class Archive, class F, class S>
void load_construct_data(Archive & ar, ::std::pair<F, S> * p,
                         unsigned int const file_version) {

  // note: we remove any const-ness on the first argument. The reason is that
  // for STL maps, the type saved is pair<const key, T). We remove
  // the const-ness in order to be able to load it.
  typedef typename boost::remove_const<F>::type typef;
  boost::serialization::detail::stack_construct<Archive, typef> first(
      ar, file_version);
  ar >> boost::serialization::make_nvp("first", first.reference());

  boost::serialization::detail::stack_construct<Archive, S> second(
      ar, file_version);
  ar >> boost::serialization::make_nvp("second", second.reference());

  // now we can create a new pair
  ::new (p)::std::pair<F, S>(boost::move(first.reference()),
                             boost::move(second.reference()));

  // make sure that addresses are reset
  ar.reset_object_address(&p->first, first.address());
  ar.reset_object_address(&p->second, second.address());
}

/// specialization of is_bitwise_serializable for pairs
template <class T, class U>
struct is_bitwise_serializable<std::pair<T,U> >
 : public mpl::and_<is_bitwise_serializable< T >,is_bitwise_serializable<U> >
{
};

} // serialization
} // namespace boost

#endif // BOOST_SERIALIZATION_UTILITY_HPP
