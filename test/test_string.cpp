#include <boost/config.hpp>
#include <boost/serialization/string.hpp>

#include <boost/archive/archive_exception.hpp>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <sstream>
#include <algorithm>

// -----------------------------------
#include <iostream>
#include <thread>

#include "test_tools.hpp"

// Test with some unicode characters
template <typename T>
struct TestTraits
{
};
template <>
struct TestTraits<char>
{
	static constexpr char DefaultValue[] = "Default";
	static constexpr char TestValue[] =
		u8"Hello boost! The author of boost::serialization is Robert Ramey.\n"
		u8"Hallo boost! Der Autor von boost::serialization ist Robert Ramey.\n"
		u8"你好 boost！ boost::serialization 由 Robert Ramey 编写。";
};

#ifndef BOOST_NO_STD_WSTRING
template <>
struct TestTraits<wchar_t>
{
	static constexpr wchar_t DefaultValue[]  = L"Default";
	static constexpr wchar_t TestValue[]  =
		L"Hello boost! The author of boost::serialization is Robert Ramey.\n"
		L"Hallo boost! Der Autor von boost::serialization ist Robert Ramey.\n"
		L"你好 boost！ boost::serialization 由 Robert Ramey 编写。";
};
#endif

template <typename String>
struct Data
{
	template<class Archive>
	void serialize(Archive& ar, const unsigned int /* version */)
	{
		ar& BOOST_SERIALIZATION_NVP(value);
		ar& BOOST_SERIALIZATION_NVP(test);
	}

	bool operator==(const Data& rhs) const
	{
		// compare by value as the capacity is irrelevant
		return std::equal(std::begin(value), std::end(value), std::begin(rhs.value)) == true &&
			test == rhs.test;
	}

	String value = TestTraits<typename String::value_type>::DefaultValue;
	int test = 1;
};

template <typename Archive, typename T>
std::string ToString(const Data<T>& data)
{
	std::ostringstream oss(std::ios::binary);

	{
		Archive oa(oss);
		oa << BOOST_SERIALIZATION_NVP(data);
	}

	return oss.str();
}

template <typename Archive, typename T>
Data<T> FromString(std::string& s)
{
	Data<T> data;
	std::istringstream iss(s, std::ios::binary);
	{
		Archive ia(iss);
		ia >> BOOST_SERIALIZATION_NVP(data);
	}
	return data;
}

template <typename String, typename OArchive, typename IArchive>
int PerformTest()
{
	try
	{
		Data<String> d1;
		d1.value = TestTraits<typename String::value_type>::TestValue;
		d1.test = 1;

		auto s1 = ToString<OArchive, String>(d1);
		auto r1 = FromString<IArchive, String>(s1);

		BOOST_CHECK(d1 == r1);
		// bool test = (d1 == r1);

		return 0;
	}
	catch (const std::exception& ex)
	{
		BOOST_ERROR(ex.what());
	}

	return 1;
}

int test_main( int /* argc */, char* /* argv */[] )
{
	int result = 0;

	// working
	result += PerformTest<std::string, 		boost::archive::xml_oarchive,		boost::archive::xml_iarchive>();
	result += PerformTest<std::pmr::string, boost::archive::xml_oarchive,		boost::archive::xml_iarchive>();

	result += PerformTest<std::string, 		boost::archive::text_oarchive,		boost::archive::text_iarchive>();
	result += PerformTest<std::pmr::string, boost::archive::text_oarchive,		boost::archive::text_iarchive>();

	result += PerformTest<std::string, 		boost::archive::binary_oarchive,	boost::archive::binary_iarchive>();
	result += PerformTest<std::pmr::string, boost::archive::binary_oarchive,	boost::archive::binary_iarchive>();

#ifndef BOOST_NO_STD_WSTRING
	result += PerformTest<std::wstring, 	boost::archive::text_oarchive, 		boost::archive::text_iarchive>();
	result += PerformTest<std::pmr::wstring,boost::archive::text_oarchive, 		boost::archive::text_iarchive>();

	result += PerformTest<std::wstring, 	boost::archive::binary_oarchive,	boost::archive::binary_iarchive>();
	result += PerformTest<std::pmr::wstring,boost::archive::binary_oarchive,	boost::archive::binary_iarchive>();

	// fails but why?
	//result += PerformTest<std::wstring, 	boost::archive::xml_oarchive, 		boost::archive::xml_iarchive>();
	//result += PerformTest<std::pmr::wstring, boost::archive::xml_oarchive, boost::archive::xml_iarchive>();
#endif // BOOST_NO_STD_WSTRING

	return result == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
