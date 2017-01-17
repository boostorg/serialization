#include <vector>
#include <sstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/binary_object.hpp>
#include "test_tools.hpp"

struct Data
{
    using value_type = char;

    template<typename Archiver>
    void save(Archiver& ar, unsigned) const
    {
      ar & value_.size();
      ar & boost::serialization::make_binary_object( value_.data(), value_.size()*sizeof(value_type) );
    }

    template<typename Archiver>
    void load(Archiver& ar, unsigned)
    {
      auto tmp = value_.size();
      ar & tmp;
      value_.resize(tmp);
      ar & boost::serialization::make_binary_object( value_.data(), value_.size()*sizeof(value_type) );
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();

    std::vector<value_type> value_;
};


int test_main( int /* argc */, char* /* argv */[] )
{
    std::stringstream ss;

    {
      Data d;
      d.value_.push_back('t');
      d.value_.push_back('e');
      d.value_.push_back('s');
      d.value_.push_back('t');

      boost::archive::binary_oarchive arch{ss};
      arch << d;
    }

    Data d;

    boost::archive::binary_iarchive arch{ss};
    arch >> d;

    BOOST_CHECK( d.value_.at(0) == 't' );
    BOOST_CHECK( d.value_.at(1) == 'e' );
    BOOST_CHECK( d.value_.at(2) == 's' );
    BOOST_CHECK( d.value_.at(3) == 't' );
    return EXIT_SUCCESS;
}
