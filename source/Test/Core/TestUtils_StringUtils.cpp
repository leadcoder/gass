#include "Core/Utils/GASSStringUtils.h"
#include "Core/Reflection/GASSProperty.h"

#include "catch.hpp"

template<typename TYPE>
void TestStream(const TYPE value, const std::string &expected_string)
{
	std::string out_str;
	TYPE out_value;
	bool success = GASS::StringUtils::GetStringFromValue(value, out_str);
	REQUIRE(success == true);
	REQUIRE(out_str == expected_string);
	success = GASS::StringUtils::GetValueFromString(out_value, expected_string);
	REQUIRE(out_value == value);
}

template<typename TYPE>
void CheckStringToValue(const std::string &string_to_parse, const TYPE &expected_value)
{
	TYPE out_value;
	bool success = GASS::StringUtils::GetValueFromString(out_value, string_to_parse);
	REQUIRE(success == true);
	REQUIRE(out_value == expected_value);
}


template<typename TYPE>
void CheckStringRoundTrip(const TYPE value)
{
	std::string str;
	bool success = GASS::StringUtils::GetStringFromValue(value, str);
	REQUIRE(success == true);
	TYPE out_value;
	success = GASS::StringUtils::GetValueFromString(out_value, str);
	REQUIRE(success == true);
	REQUIRE(out_value == value);
}

TEST_CASE("Test replacing different strings")
{
	SECTION("Test all empty strings")
	{
		REQUIRE_THROWS(GASS::StringUtils::Replace("", "", ""));
	}

	SECTION("Test that replacement string contain find string will throw to avoid infinite loop")
	{
		REQUIRE_THROWS(GASS::StringUtils::Replace("test string", "ab", "12ab34"));
	}

	SECTION("Test that find string not occur in string to search in")
	{
		REQUIRE(GASS::StringUtils::Replace("test string", "ab", "1234") == "test string");
	}

	SECTION("Test to upper/lower with empty strings")
	{
		REQUIRE(GASS::StringUtils::ToLower("") == "");

		REQUIRE(GASS::StringUtils::ToUpper("") == "");
	}

	SECTION("Test case insensitive comparision, which in turn uses GASS::StringUtils::ToLower()")
	{
		REQUIRE(GASS::StringUtils::CompareNoCase("ab12cd", "aB12Cd") == 0);
		REQUIRE(GASS::StringUtils::CompareNoCase("", "aB12Cd") < 0);
		REQUIRE(GASS::StringUtils::CompareNoCase("aB12Cd", "") > 0);
	}

	SECTION("Test stringstream string")
	{
		CheckStringRoundTrip<std::string>("Test1");
		CheckStringRoundTrip<std::string>(" Test 2 ");
	}

	SECTION("Test stringstream bool")
	{
		CheckStringRoundTrip<bool>(true);
		CheckStringRoundTrip<bool>(false);

		CheckStringToValue<bool>("true", true);
		CheckStringToValue<bool>("false", false);
		CheckStringToValue<bool>("  true", true);
		CheckStringToValue<bool>("  false ", false);

		CheckStringToValue<bool>("0", false);
		CheckStringToValue<bool>("1", true);

		CheckStringToValue<bool>("0 ", false);
		CheckStringToValue<bool>(" 1 ", true);
	}

	SECTION("Test stringstream int")
	{
		CheckStringRoundTrip<int>(11234);
		CheckStringRoundTrip<int>(-11234);
	}

	SECTION("Test stringstream double")
	{
		//Within std::numeric_limits<float>::digits109+1
		CheckStringRoundTrip<double>(11234.1345);
		CheckStringRoundTrip<double>(-11234.1345);
		CheckStringRoundTrip<double>(0);

		TestStream<double>(11234.1345, "11234.1345");
		TestStream<double>(-11234.1345, "-11234.1345");
		TestStream<double>(0, "0");

		//test that trailing zeros are removed/clipped
		TestStream<double>(11234.00, "11234");

		CheckStringToValue<double>("1.0", 1.0);
		CheckStringToValue<double>("   2.100", 2.1);
		CheckStringToValue<double>("   1.200   ", 1.2);

		//Test precision (numeric limit)
		std::string out_str;
		bool success = GASS::StringUtils::GetStringFromValue(double(123456789.12345679), out_str);
		REQUIRE(success == true);
		REQUIRE(out_str == "123456789.1234568");
	}
	
	SECTION("Test stringstream float")
	{
		CheckStringRoundTrip<float>(123.456f);
		CheckStringRoundTrip<float>(-123.456f);
		CheckStringRoundTrip<float>(0.0f);

		TestStream<float>(134.345f, "134.345");
		TestStream<float>(-134.345f, "-134.345");

		std::string out_str;

		//test float round
		bool success = GASS::StringUtils::GetStringFromValue(float(1234.3456), out_str);
		REQUIRE(success == true);
		REQUIRE(out_str == "1234.346");
	}

	SECTION("Test stringstream Vec3")
	{
		CheckStringRoundTrip<GASS::Vec3>(GASS::Vec3(1, 0, 2));
		CheckStringRoundTrip<GASS::Vec3>(GASS::Vec3(12314.454, 2, 345.3));
		CheckStringRoundTrip<GASS::Vec3>(GASS::Vec3(-1234414.458, 344.5666, 34345456.66));
	
		CheckStringToValue("1.0 2.0 4.2", GASS::Vec3(1.0, 2.0, 4.2));
		CheckStringToValue("   1.0    2.0  4.2 ", GASS::Vec3(1.0, 2.0, 4.2));
	}

	SECTION("Test stringstream std::vector<float>")
	{
		std::vector<float> test_vec;
		//test empty vector!
		CheckStringRoundTrip<std::vector<float> >(test_vec);

		test_vec.push_back(1.0f);
		test_vec.push_back(1.3f);
		test_vec.push_back(15.0f);
		test_vec.push_back(235.0f);
		test_vec.push_back(23.234f);
		CheckStringRoundTrip<std::vector<float> >(test_vec);

		CheckStringToValue<std::vector<float> >("   1.00  1.3  15.00 235           23.234", test_vec);
		
		//trailing ws can not be handled...we need to cont. parsing until eof, the returned vector will be correct but stream will fail:(
		CheckStringToValue<std::vector<float> >("   1.00  1.3  15.00 235           23.234 ", test_vec);
	}

	SECTION("Test stringstream std::vector<int>")
	{
		std::vector<int> test_vec;
		test_vec.push_back(1);
		test_vec.push_back(2);
		test_vec.push_back(15);
		test_vec.push_back(234235);
		test_vec.push_back(-1230);
		CheckStringRoundTrip<std::vector<int> >(test_vec);
	}

	SECTION("Test stringstream std::vector<GASS::Vec3>")
	{
		std::vector<GASS::Vec3> test_vec;
		test_vec.push_back(GASS::Vec3(1,0,2));
		test_vec.push_back(GASS::Vec3(10.5, -20, 2.2));
		test_vec.push_back(GASS::Vec3(1245.23, 343.5, 56.7));
		test_vec.push_back(GASS::Vec3(1, -100, 2.22222));
		CheckStringRoundTrip<std::vector<GASS::Vec3> >(test_vec);
	}

	SECTION("Test stringstream std::vector<std::string>")
	{
		std::vector<std::string> test_vec;
		test_vec.push_back("t1");
		test_vec.push_back("t2");
		test_vec.push_back("t3");
		CheckStringRoundTrip<std::vector<std::string> >(test_vec);

		//Strings with ws (in vector) not supported due to fact that ws is vector-element-delimiter 
		//and we want to be consistent when parsing vectors
	//	test_vec.clear();
	//	test_vec.push_back("t1");
	//	test_vec.push_back(" t2 ");
	//	test_vec.push_back(" t 3 ");
	//	CheckStringRoundTrip<std::vector<std::string> >(test_vec);
	}

	SECTION("Test stringstream std::map<std::string,float>")
	{
		std::map<std::string,float> test_map;
		test_map["key1"] = 10.0;
		test_map["key2"] = 12.0;
		test_map["key3"] = 13.0;
		CheckStringRoundTrip<std::map<std::string, float> >(test_map);
	}
}