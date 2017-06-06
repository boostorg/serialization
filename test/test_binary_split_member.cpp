#include <vector>
#include <sstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/binary_object.hpp>
#include "test_tools.hpp"

struct Data
{
    typedef char value_type;

    template<typename Archiver>
    void save(Archiver& ar, unsigned) const
    {
      ar & value_.size();
      ar & boost::serialization::make_binary_object( value_.data(), value_.size()*sizeof(value_type) );
    }

    template<typename Archiver>
    void load(Archiver& ar, unsigned)
    {
      std::vector<value_type>::size_type size;
      ar & size;
      value_.resize(size);
      ar & boost::serialization::make_binary_object( value_.data(), value_.size()*sizeof(value_type) );
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();

    std::vector<value_type> value_;
};


int test_main( int /* argc */, char* /* argv */[] )
{
    std::stringstream ss;

    Data in;
    in.value_.push_back('t');
    in.value_.push_back('e');
    in.value_.push_back('s');
    in.value_.push_back('t');
    {
      boost::archive::binary_oarchive arch(ss);
      arch << static_cast<Data const&>(in);
    }

    Data out;
    {
      boost::archive::binary_iarchive arch(ss);
      arch >> out;
    }
    BOOST_CHECK( in.value_ == out.value_ );
    return EXIT_SUCCESS;
}
