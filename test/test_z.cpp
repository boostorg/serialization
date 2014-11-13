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
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

struct dummy {
    template<class Archive>
    void serialize(Archive& ar, unsigned int version) {}
};

int main() {
    // One-level container 
    // For comparison only. This shows the expected behaviour. 
    {
        std::cout << "One-level vector:" << std::endl;
        std::vector<dummy> l(1);
        dummy* pd = &l.back();
        {
            std::ofstream ofs("one_level.xml");
            boost::archive::xml_oarchive oa(ofs);
            oa << BOOST_SERIALIZATION_NVP(l) << BOOST_SERIALIZATION_NVP(pd);
        }
        {
            std::ifstream ifs("one_level.xml");
            boost::archive::xml_iarchive ia(ifs);
            ia >> BOOST_SERIALIZATION_NVP(l) >> BOOST_SERIALIZATION_NVP(pd);
        }
        std::cout << &l.back() << " " << pd << std::endl;
    }

    std::cout << std::endl;

    // Two-level container 
    // This is buggy. 
    {
        std::cout << "Two-level vector:" <<std::endl;
        std::vector<std::vector<dummy> > l(1, std::vector<dummy>(1));
        dummy* pd = &l.back().back();
        std::cout << pd << std::endl;
        {
            std::ofstream ofs("two_level.xml");
            boost::archive::xml_oarchive oa(ofs);
            oa << BOOST_SERIALIZATION_NVP(l) << BOOST_SERIALIZATION_NVP(pd);
        }
        {
            std::ifstream ifs("two_level.xml");
            boost::archive::xml_iarchive ia(ifs);
            ia >> BOOST_SERIALIZATION_NVP(l) >> BOOST_SERIALIZATION_NVP(pd);
        }
        std::cout << &l.back().back() << " " << pd << std::endl;
    }

    return 0;
}

