#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <vector>
#include <fstream>

using namespace std;

struct Data {
  vector<int> v;
};

namespace boost {
  namespace serialization {

    template<class Archive>
      void serialize(Archive & a, Data &d, const unsigned int version)
      {
        a & d.v;
      }

  }
}

int main(int argc, char **argv)
{
  if (argc > 10) {
    ifstream f("/dev/null");
    boost::archive::text_iarchive a(f);
    Data d;
    a >> d;
  } else {
    ofstream f("/dev/null");
    boost::archive::text_oarchive a(f);
    Data d;
    a << d;
  }
  return 0;
}
