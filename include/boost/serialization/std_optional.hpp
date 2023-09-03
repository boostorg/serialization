#ifndef BOOST_SERIALIZATION_STD_OPTIONAL_HPP_
#define BOOST_SERIALIZATION_STD_OPTIONAL_HPP_

#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// optional.hpp - non-intrusive serialization of optional types
//
// copyright (c) 2019 Samuel Debionne, ESRF
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org for updates, documentation, and revision history.
//
// Widely inspired form boost::optional serialization
//

#include <optional>

#include <boost/config.hpp>

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/level.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/serialization/detail/stack_constructor.hpp>
#include <boost/serialization/detail/is_default_constructible.hpp>
#include <boost/serialization/force_include.hpp>

// function specializations must be defined in the appropriate
// namespace - boost::serialization
namespace boost {
	namespace serialization {

		template<class Archive, class T>
		void save(
			Archive& ar,
			const std::optional< T >& t,
			const unsigned int /*version*/
		) {
			// It is an inherent limitation to the serialization of optional.hpp
			// that the underlying type must be either a pointer or must have a
			// default constructor.  It's possible that this could change sometime
			// in the future, but for now, one will have to work around it.  This can
			// be done by serialization the optional<T> as optional<T *>
#if ! defined(BOOST_NO_CXX11_HDR_TYPE_TRAITS)
			BOOST_STATIC_ASSERT(
				boost::serialization::detail::is_default_constructible<T>::value
				|| boost::is_pointer<T>::value
			);
#endif
			const bool tflag = t.has_value();
			ar << boost::serialization::make_nvp("has_value", tflag);
			if (tflag) {
				ar << boost::serialization::make_nvp("value", *t);
			}
		}

		template<class Archive, class T>
		void load(
			Archive& ar,
			std::optional< T >& t,
			const unsigned int version
		) {
			bool tflag;
			ar >> boost::serialization::make_nvp("has_value", tflag);
			if (!tflag) {
				t.reset();
				return;
			}

			if (!t.has_value())
				t = T();
			ar >> boost::serialization::make_nvp("value", *t);
		}

		template<class Archive, class T>
		void serialize(
			Archive& ar,
			std::optional< T >& t,
			const unsigned int version
		) {
			boost::serialization::split_free(ar, t, version);
		}

		template<class T>
		struct version<std::optional<T> > {
			BOOST_STATIC_CONSTANT(int, value = 1);
		};

	} // serialization
} // boost

#endif // BOOST_SERIALIZATION_STD_OPTIONAL_HPP_
