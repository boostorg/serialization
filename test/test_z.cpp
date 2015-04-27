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
