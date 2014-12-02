/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_z.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <vector>
#include <fstream>

#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

struct dummy {
    int m_id;
    template<class Archive>
    void serialize(Archive& ar, unsigned int version) {
        ar & m_id;
    }
    dummy() :
        m_id(0)
    {}
    bool operator==(const dummy & rhs){
        return m_id == rhs.m_id;
    }
};

int main() {
    // Two-level container
    // This is buggy. 
    std::cout << "Two-level vector:" <<std::endl;
    std::vector<std::vector<dummy> > l(1, std::vector<dummy>(1));
    l[0][0].m_id = 42;
    dummy* pd = &l.back().back();
    std::cout << pd << std::endl;
    {
        std::ofstream ofs("two_level.txt");
        boost::archive::text_oarchive oa(ofs);
        oa << l;
        oa << pd;
    }
    dummy* pd1;
    std::vector<std::vector<dummy> > l1;
    {
        std::ifstream ifs("two_level.txt");
        boost::archive::text_iarchive ia(ifs);
        ia >> l1;
        dummy * x = & l1[0][0];
        ia >> pd1;
        assert(pd1 == x);
    }
    std::cout << pd1 << std::endl;
    assert(l[0][0] == l1[0][0]);
    assert(*pd == *pd1);

    return 0;
}