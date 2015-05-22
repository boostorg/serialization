#if 0

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/tuple/tuple.hpp>

#include <iostream>
#include <vector>
#include <sstream>

namespace boost {
  namespace serialization {

    template<typename archive, typename T0, typename T1, typename T2>
    void serialize(archive & ar, boost::tuple<T0, T1, T2> & tpl, unsigned int) {
      using namespace boost;
      using boost::serialization::make_nvp;

      ar
	& make_nvp("tpl_0", boost::get<0>(tpl))
	& make_nvp("tpl_1", boost::get<1>(tpl))
	& make_nvp("tpl_2", boost::get<2>(tpl));
    }

  } /* namespace serialization */
} /* namespace boost */

#include <stdio.h>
#include <ciso646>
int main() {
	printf("Hello World\n");
	printf("libc++ version = %i\n", _LIBCPP_VERSION);
	printf("clang version = %i\n", __cplusplus);

  std::vector<boost::tuple<double, double, double> > t_vec, t_vec2(1);
  std::ostringstream oss;

  t_vec.push_back(boost::tuple<double, double, double>(1.,2.,3.));
  std::cout << "t_vec.size() = " << t_vec.size() << std::endl;

  // save data to archive
  {
    boost::archive::xml_oarchive oa(oss);
    // write class instance to archive
    oa & BOOST_SERIALIZATION_NVP(t_vec);
    // archive and stream closed when destructors are called
    // Data is now in oss
  }

  // load data from archive
  {
    std::istringstream iss(oss.str());
    boost::archive::xml_iarchive ia(iss);
    // read class state from archive
    ia & BOOST_SERIALIZATION_NVP(t_vec2);
    // archive and stream closed when destructors are called
  }

  std::cout << "t_vec2.size() = " << t_vec2.size() << std::endl;
}


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
private:
    friend boost::serialization::access;
    friend std::vector<dummy>;
    friend int main();
    int m_id;
    template<class Archive>
    void serialize(Archive& ar, unsigned int version) {
        ar & m_id;
    }
    dummy() :
        m_id(0)
    {}
    dummy(int id) :
        m_id(id)
    {}
public:
    bool operator==(const dummy & rhs){
        return m_id == rhs.m_id;
    }
    dummy & operator=(const int rhs){
        m_id = rhs;
        return *this;
    }
};

int main() {
    // Two-level container
    // This is buggy. 
    std::cout << "Two-level vector:" <<std::endl;
    std::vector<std::vector<dummy> > l(1, std::vector<dummy>{1});
    l[0][0] = 42;
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
        ia >> pd1;
    }
    std::cout << pd1 << std::endl;
    assert(l[0][0] == l1[0][0]);
    assert(*pd == *pd1);

    return 0;
}

#endif

