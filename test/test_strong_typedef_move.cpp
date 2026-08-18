/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_strong_typedef_move.cpp

// Copyright 2026 Gennaro Prota.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// Declaring a copy constructor and a copy assignment operator stops the
// compiler from declaring the move ones, so a strong typedef used to copy
// the value it wraps where a move was asked for.  That was silent: the code
// compiled and gave the right answer, only slowly, which is why it wants a
// test that counts the operations rather than one that merely compiles.

// Reported by bweed-mathworks in
// https://github.com/boostorg/serialization/issues/341, with the two members
// to add.  Thanks!

#include <boost/core/lightweight_test.hpp>
#include <boost/serialization/strong_typedef.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_nothrow_move_assignable.hpp>
#include <boost/type_traits/is_nothrow_move_constructible.hpp>

#include <utility>

// Says which of the two happened to it.  The counters are global so that a
// check reads as "this many moves, no copies" without having to reason
// about which object ended up holding the tally.
struct counted {
    static int copies;
    static int moves;
    counted(){}
    counted(const counted &){
        ++copies;
    }
    counted(counted &&){
        ++moves;
    }
    counted & operator=(const counted &){
        ++copies;
        return *this;
    }
    counted & operator=(counted &&){
        ++moves;
        return *this;
    }
    bool operator==(const counted &) const {
        return true;
    }
    bool operator<(const counted &) const {
        return false;
    }
};

int counted::copies = 0;
int counted::moves = 0;

BOOST_STRONG_TYPEDEF(counted, strong_counted)
BOOST_STRONG_TYPEDEF(int, strong_int)

static void reset(){
    counted::copies = 0;
    counted::moves = 0;
}

// A strong typedef of a type which can throw must not claim it cannot, and
// one of a type which cannot must not throw away the guarantee.
BOOST_STATIC_ASSERT(boost::is_nothrow_move_constructible<strong_int>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_assignable<strong_int>::value);
BOOST_STATIC_ASSERT(! boost::is_nothrow_move_constructible<strong_counted>::value);
BOOST_STATIC_ASSERT(! boost::is_nothrow_move_assignable<strong_counted>::value);

static void test_move_construction(){
    reset();
    strong_counted a;
    strong_counted b(std::move(a));
    (void) b;
    BOOST_TEST_EQ(counted::moves, 1);
    BOOST_TEST_EQ(counted::copies, 0);
}

static void test_move_assignment(){
    reset();
    strong_counted a;
    strong_counted b;
    b = std::move(a);
    BOOST_TEST_EQ(counted::moves, 1);
    BOOST_TEST_EQ(counted::copies, 0);
}

// Copying has to go on working, and go on being a copy.
static void test_copy_still_copies(){
    reset();
    strong_counted a;
    strong_counted b(a);
    strong_counted c;
    c = a;
    BOOST_TEST_EQ(counted::copies, 2);
    BOOST_TEST_EQ(counted::moves, 0);
}

// The value has to survive the move, not merely be moved.
static void test_value_is_carried(){
    strong_int a(42);
    strong_int b(std::move(a));
    BOOST_TEST_EQ(b.t, 42);

    strong_int c;
    c = std::move(b);
    BOOST_TEST_EQ(c.t, 42);
}

int
main(){
    test_move_construction();
    test_move_assignment();
    test_copy_still_copies();
    test_value_is_carried();
    return boost::report_errors();
}
