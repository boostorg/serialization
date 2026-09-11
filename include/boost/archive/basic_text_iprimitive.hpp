#ifndef BOOST_ARCHIVE_BASIC_TEXT_IPRIMITIVE_HPP
#define BOOST_ARCHIVE_BASIC_TEXT_IPRIMITIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_text_iprimitive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// archives stored as text - note these are templated on the basic
// stream templates to accommodate wide (and other?) kind of characters
//
// Note the fact that on libraries without wide characters, ostream is
// not a specialization of basic_ostream which in fact is not defined
// in such cases.   So we can't use basic_ostream<IStream::char_type> but rather
// use two template parameters

#include <ios>
#include <limits>
#include <locale>
#include <string>
#include <cstddef> // size_t
#include <streambuf>
#include <string>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
    #if ! defined(BOOST_DINKUMWARE_STDLIB) && ! defined(__SGI_STL_PORT)
        using ::locale;
    #endif
} // namespace std
#endif

#include <boost/io/ios_state.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/static_assert.hpp>

#include <boost/detail/workaround.hpp>
#if BOOST_WORKAROUND(BOOST_DINKUMWARE_STDLIB, == 1)
#include <boost/archive/dinkumware.hpp>
#endif
#include <boost/serialization/throw_exception.hpp>
#include <boost/archive/codecvt_null.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/archive/basic_streambuf_locale_saver.hpp>
#include <boost/archive/detail/stream_position_message.hpp>
#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {

/////////////////////////////////////////////////////////////////////////
// class basic_text_iarchive - load serialized objects from a input text stream
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4244 4267 )
#endif

template<class IStream>
class BOOST_SYMBOL_VISIBLE basic_text_iprimitive {
protected:
    IStream &is;
    io::ios_flags_saver flags_saver;
    io::ios_precision_saver precision_saver;

    #ifndef BOOST_NO_STD_LOCALE
    // note order! - if you change this, libstd++ will fail!
    // a) create new locale with new codecvt facet
    // b) save current locale
    // c) change locale to new one
    // d) use stream buffer
    // e) change locale back to original
    // f) destroy new codecvt facet
    boost::archive::codecvt_null<typename IStream::char_type> codecvt_null_facet;
    std::locale archive_locale;
    basic_istream_locale_saver<
        typename IStream::char_type,
        typename IStream::traits_type
    > locale_saver;
    #endif

    // Whether a value of T can be an infinity or a NaN, and so may reach
    // us written as letters rather than as digits.
    template<class T>
    struct has_non_finite {
        typedef typename mpl::bool_<
            std::numeric_limits<T>::has_infinity
            || std::numeric_limits<T>::has_quiet_NaN
        >::type type;
    };

    // Takes a leading sign, if there is one, and says whether it was a
    // minus.
    bool take_minus_sign(){
        typedef typename IStream::traits_type traits_type;
        typedef typename IStream::char_type char_type;

        std::basic_streambuf<char_type, traits_type> * const sb = is.rdbuf();
        if(NULL == sb){
            return false;
        }
        is >> std::ws;
        const typename traits_type::int_type c = sb->sgetc();
        if(traits_type::eq_int_type(c, traits_type::to_int_type(char_type('-')))
        || traits_type::eq_int_type(c, traits_type::to_int_type(char_type('+')))){
            return traits_type::eq_int_type(
                sb->sbumpc(), traits_type::to_int_type(char_type('-'))
            );
        }
        return false;
    }

    template<class T>
    void load_impl(T & t, boost::mpl::bool_<false> &){
        if(is >> t)
            return;
        const std::string message =
            detail::stream_position_message(is.rdbuf());
        boost::serialization::throw_exception(
            archive_exception(
                archive_exception::input_stream_error,
                message.c_str()
            )
        );
    }

    // An infinity is written as "inf" and a NaN as "nan", because that is
    // what the stream writes, and the extraction of a floating point number
    // then refuses both, so an archive the library wrote itself would not
    // load.  Read the letters here rather than change what is written, so
    // that archives already in existence start loading (issue #386).
    template<class T>
    void load_impl(T & t, boost::mpl::bool_<true> &){
        typedef typename IStream::traits_type traits_type;
        typedef typename IStream::char_type char_type;

        const bool negative = take_minus_sign();
        if(is >> t){
            if(negative){
                t = -t;
            }
            return;
        }
        is.clear();

        // Only the letters are taken, and not everything up to the next
        // space, because an XML archive ends a value with a tag.  The
        // terminator is left where it is for the same reason.
        std::basic_streambuf<char_type, traits_type> * const sb = is.rdbuf();
        std::string token;
        for(;;){
            const typename traits_type::int_type c = sb->sgetc();
            if(traits_type::eq_int_type(c, traits_type::eof())){
                break;
            }
            const char_type letter = traits_type::to_char_type(c);
            if(! ((char_type('a') <= letter && letter <= char_type('z'))
               || (char_type('A') <= letter && letter <= char_type('Z')))){
                break;
            }
            token += char(char(letter) | 0x20);   // ASCII, so this lowers it
            sb->sbumpc();
        }
        // A NaN may carry a parenthesised payload, as in the "nan(ind)" the
        // Microsoft library writes, which has to come away with it.
        if("nan" == token
        && traits_type::eq_int_type(
               sb->sgetc(), traits_type::to_int_type(char_type('('))
           )
        ){
            while(! traits_type::eq_int_type(
                      sb->sbumpc(), traits_type::to_int_type(char_type(')'))
                  )){
                if(traits_type::eq_int_type(sb->sgetc(), traits_type::eof())){
                    break;
                }
            }
        }

        if(("inf" == token || "infinity" == token)
        && std::numeric_limits<T>::has_infinity){
            t = std::numeric_limits<T>::infinity();
        }
        else if("nan" == token && std::numeric_limits<T>::has_quiet_NaN){
            t = std::numeric_limits<T>::quiet_NaN();
        }
        else{
            boost::serialization::throw_exception(
                archive_exception(archive_exception::input_stream_error)
            );
        }
        if(negative){
            t = -t;
        }
    }

    template<class T>
    void load(T & t)
    {
        typename has_non_finite<T>::type tag;
        load_impl(t, tag);
    }

    void load(char & t)
    {
        short int i;
        load(i);
        t = i;
    }
    void load(signed char & t)
    {
        short int i;
        load(i);
        t = i;
    }
    void load(unsigned char & t)
    {
        unsigned short int i;
        load(i);
        t = i;
    }

    #ifndef BOOST_NO_INTRINSIC_WCHAR_T
    void load(wchar_t & t)
    {
        BOOST_STATIC_ASSERT(sizeof(wchar_t) <= sizeof(int));
        int i;
        load(i);
        t = i;
    }
    #endif
    BOOST_ARCHIVE_OR_WARCHIVE_DECL
    basic_text_iprimitive(IStream  &is, bool no_codecvt);
    BOOST_ARCHIVE_OR_WARCHIVE_DECL
    ~basic_text_iprimitive();
public:
    BOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load_binary(void *address, std::size_t count);
};

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

} // namespace archive
} // namespace boost

#include <boost/archive/detail/abi_suffix.hpp> // pop pragmas

#endif // BOOST_ARCHIVE_BASIC_TEXT_IPRIMITIVE_HPP
