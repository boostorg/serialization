// Copyright (c) 2001 Ronald Garcia, Indiana University (garcia@osl.iu.edu)
// Andrew Lumsdaine, Indiana University (lums@osl.iu.edu).
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_ARCHIVE_DETAIL_UTF8_CODECVT_FACET_HPP
#define BOOST_ARCHIVE_DETAIL_UTF8_CODECVT_FACET_HPP

#include <boost/config.hpp>

#define BOOST_UTF8_BEGIN_NAMESPACE \
    namespace boost { namespace archive { namespace detail {
#define BOOST_UTF8_END_NAMESPACE }}}

#ifdef BOOST_NO_CXX11_HDR_CODECVT
    #include <boost/locale/utf8_codecvt.hpp>
    BOOST_UTF8_BEGIN_NAMESPACE
        typedef boost::locale::utf8_codecvt<wchar_t> utf8_codecvt_facet;
    BOOST_UTF8_END_NAMESPACE
#else
    #include <codecvt>
    BOOST_UTF8_BEGIN_NAMESPACE
        typedef std::codecvt_utf8<wchar_t> utf8_codecvt_facet;
    BOOST_UTF8_END_NAMESPACE
#endif // BOOST_NO_CXX11_HDR_CODECVT

#undef BOOST_UTF8_BEGIN_NAMESPACE
#undef BOOST_UTF8_END_NAMESPACE

#endif // BOOST_ARCHIVE_DETAIL_UTF8_CODECVT_FACET_HPP

