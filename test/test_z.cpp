#include <fstream>
#include <string>

#include <boost/archive/xml_woarchive.hpp>

#include "test_tools.hpp"

int test_main(int, char *argv[])
{
    const char * testfile = boost::archive::tmpnam(NULL);
	std::wofstream ofs(testfile);

	std::string s1 = "kkk";
	std::wstring w1 = L"kkk";
	std::wstring w2 = L"апр"; // some non-lati1 (for example russians) letters
    {
        boost::archive::xml_woarchive oa(ofs);
        oa << boost::serialization::make_nvp("key1", s1);
        oa << boost::serialization::make_nvp("key2", w1);
        oa << boost::serialization::make_nvp("key3", w2); // here exception is thrown
    }
	return 0;
}
