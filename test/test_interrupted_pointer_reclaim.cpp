/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_interrupted_pointer_reclaim.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// When loading through a pointer throws part way, delete_created_pointers()
// has to reclaim the object which was left half loaded, and leave alone the
// objects an owning smart pointer has already taken over.

// Reported by hanzotutu in
// https://github.com/boostorg/serialization/issues/260, together with a
// careful matrix of raw pointer, shared_ptr, single object and vector
// cases.  That analysis is what pinned the bug down, and it shaped the
// cases tested here.  Thanks for the effort which went into it!

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <memory>
#include <vector>
#include <stdexcept>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::remove;
}
#endif

#include <boost/core/no_exceptions_support.hpp>
#include <boost/serialization/throw_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>

#include "test_tools.hpp"

// Counts live instances so that a leak shows up without a leak checker.
class base
{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & /* ar */, const unsigned int /* file_version */){
    }
public:
    static int count;
    base(){++count;}
    virtual ~base(){--count;}
};
int base::count = 0;

// Which derived::load call throws, or -1 for none.
static int throw_at = -1;
static int load_calls = 0;

class derived : public base
{
    friend class boost::serialization::access;
    int x;
    template<class Archive>
    void save(Archive & ar, const unsigned int /* file_version */) const {
        ar << boost::serialization::make_nvp(
            "base", boost::serialization::base_object<base>(*this));
        ar << BOOST_SERIALIZATION_NVP(x);
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */){
        ar >> boost::serialization::make_nvp(
            "base", boost::serialization::base_object<base>(*this));
        ar >> BOOST_SERIALIZATION_NVP(x);
        if(load_calls++ == throw_at){
            boost::serialization::throw_exception(
                std::runtime_error("interrupted load")
            );
        }
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
    derived() : x(0) {}
    explicit derived(int x_) : x(x_) {}
};

BOOST_CLASS_EXPORT(derived)
BOOST_SERIALIZATION_SHARED_PTR(derived)

typedef std::vector<std::shared_ptr<base> > vector_type;

// The first element is left empty on purpose: a null pointer must not make
// the archive lose track of the objects loaded around it.
static void save(const char * testfile, std::size_t n){
    vector_type v;
    v.push_back(std::shared_ptr<base>());
    for(std::size_t i = 1; i < n; ++i){
        v.push_back(std::make_shared<derived>(static_cast<int>(i)));
    }
    test_ostream os(testfile, TEST_STREAM_FLAGS);
    test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
    oa << BOOST_SERIALIZATION_NVP(v);
}

// A load interrupted in the middle of the last element must leave nothing
// behind: neither the half loaded object nor the complete ones before it.
static void test_interrupted(const char * testfile){
    load_calls = 0;
    throw_at = 2;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        vector_type v;
        BOOST_TRY{
            ia >> BOOST_SERIALIZATION_NVP(v);
            BOOST_ERROR("the interrupted load should have thrown");
        }
        BOOST_CATCH(const std::runtime_error &){
            ia.delete_created_pointers();
            v.clear();
        }
        BOOST_CATCH_END
    }
    BOOST_CHECK(0 == base::count);
}

// A load which completes is owned by the shared_ptrs alone, so a stray
// delete_created_pointers() must not free anything under them.
static void test_completed(const char * testfile, std::size_t n){
    load_calls = 0;
    throw_at = -1;
    {
        test_istream is(testfile, TEST_STREAM_FLAGS);
        test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
        vector_type v;
        ia >> BOOST_SERIALIZATION_NVP(v);
        BOOST_CHECK(n == v.size());
        BOOST_CHECK(NULL == v[0].get());
        // Every element but the empty one holds a live object.
        BOOST_CHECK(static_cast<int>(n) - 1 == base::count);
        ia.delete_created_pointers();
    }
    BOOST_CHECK(0 == base::count);
}

int
test_main(int /* argc */, char * /* argv */ [])
{
    const std::size_t n = 4;
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);

    save(testfile, n);
    BOOST_CHECK(0 == base::count);

    test_interrupted(testfile);
    test_completed(testfile, n);

    std::remove(testfile);
    return EXIT_SUCCESS;
}
