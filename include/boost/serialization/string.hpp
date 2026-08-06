#ifndef  BOOST_SERIALIZATION_STRING_HPP
#define BOOST_SERIALIZATION_STRING_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// serialization/string.hpp:
// serialization for stl string templates

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <memory>
#include <string>

#include <boost/config.hpp>
#include <boost/core/enable_if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/integral_c_tag.hpp>
#include <boost/type_traits/is_same.hpp>

#include <boost/serialization/level.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>

BOOST_CLASS_IMPLEMENTATION(std::string, boost::serialization::primitive_type)
#ifndef BOOST_NO_STD_WSTRING
BOOST_CLASS_IMPLEMENTATION(std::wstring, boost::serialization::primitive_type)
#endif

namespace boost {
namespace serialization {

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_string with an allocator of its own, std::pmr::string being the
// usual case.  The archives implement primitives for std::string and
// std::wstring only, so such a string borrows the primitive of the
// correspondingly built default allocated string.  Passing a null name to
// make_nvp keeps the representation identical to that of std::string in
// every archive, xml included, so the two interoperate.

template<class Archive, class Ch, class Tr, class Alloc>
inline typename boost::disable_if<
    boost::is_same<Alloc, std::allocator<Ch> >
>::type
save(
    Archive & ar,
    const std::basic_string<Ch, Tr, Alloc> & t,
    const unsigned int /* file_version */
){
    const std::basic_string<Ch, Tr> s(t.begin(), t.end());
    ar << boost::serialization::make_nvp(NULL, s);
}

template<class Archive, class Ch, class Tr, class Alloc>
inline typename boost::disable_if<
    boost::is_same<Alloc, std::allocator<Ch> >
>::type
load(
    Archive & ar,
    std::basic_string<Ch, Tr, Alloc> & t,
    const unsigned int /* file_version */
){
    std::basic_string<Ch, Tr> s;
    ar >> boost::serialization::make_nvp(NULL, s);
    t.assign(s.begin(), s.end());
}

// split non-intrusive serialization function template into separate
// non intrusive save/load function templates
template<class Archive, class Ch, class Tr, class Alloc>
inline typename boost::disable_if<
    boost::is_same<Alloc, std::allocator<Ch> >
>::type
serialize(
    Archive & ar,
    std::basic_string<Ch, Tr, Alloc> & t,
    const unsigned int file_version
){
    boost::serialization::split_free(ar, t, file_version);
}

// A string carries no class information and no version.  This has to
// specialize the same template BOOST_CLASS_IMPLEMENTATION does, so that the
// full specializations naming std::string and std::wstring win over it.
template<class Ch, class Tr, class Alloc>
struct implementation_level_impl<
    const std::basic_string<Ch, Tr, Alloc>
>
{
    typedef mpl::integral_c_tag tag;
    typedef mpl::int_<object_serializable> type;
    BOOST_STATIC_CONSTANT(int, value = object_serializable);
};

} // namespace serialization
} // namespace boost

#endif // BOOST_SERIALIZATION_STRING_HPP
