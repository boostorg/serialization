#include <fstream>

#include <boost/archive/xml_woarchive.hpp>
#include <boost/archive/xml_wiarchive.hpp>
#include <boost/serialization/string.hpp>

#include "test_tools.hpp"

int test_main(int, char *argv[])
{
    const char * testfile = boost::archive::tmpnam(NULL);
	std::string s1 = "kkkabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	std::wstring w1 = L"kkk";
	std::wstring w2 = L"апр"; // some non-latin (for example russians) letters
    {
        std::wofstream ofs(testfile);
        {
            boost::archive::xml_woarchive oa(ofs);
            oa << boost::serialization::make_nvp("key1", s1);
            //oa << boost::serialization::make_nvp("key2", w1);
            //oa << boost::serialization::make_nvp("key3", w2); // here exception is thrown
        }
    }
    std::string new_s1;
    //std::wstring new_w1;
    //std::wstring new_w2;
    {
        std::wifstream ifs(testfile);
        {
            boost::archive::xml_wiarchive ia(ifs);
            ia >> boost::serialization::make_nvp("key1", new_s1);
            //ia >> boost::serialization::make_nvp("key2", new_w1);
            //ia >> boost::serialization::make_nvp("key3", new_w2); // here exception is thrown
        }
    }
    BOOST_CHECK(s1 == new_s1);
    //BOOST_CHECK(w1 == new_w1);
    //BOOST_CHECK(w2 == new_w2);
	return 0;
}
