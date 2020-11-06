#include "Core/Utils/GASSEnumLookup.h"

#include "catch.hpp"

TEST_CASE("Test verifying GASSEnumLookup")
{	
	SECTION("Test added enum return a valid value")
	{
        GASS::EnumLookup enum_lookup;
        enum_lookup.Add("TestEnumName");
		REQUIRE(enum_lookup.Get("TestEnumName") == 0);
	}

    SECTION("Test retrieving enum value for invalid name should return -1")
	{
        GASS::EnumLookup enum_lookup;
        
		REQUIRE(enum_lookup.Get("TestInvalidEnumName") == -1);
	}

    SECTION("Test retrieving recently added value for given enum name")
	{
        GASS::EnumLookup enum_lookup;
        const int enum_val_1 = 1;
        const int enum_val_2 = 45;
        const int enum_val_3 = 324;

        enum_lookup.Add("EnumName1", enum_val_1);
        enum_lookup.Add("EnumName2", enum_val_2);
        enum_lookup.Add("EnumName3", enum_val_3);
        
		REQUIRE(enum_lookup.Get("EnumName1") == enum_val_1);
        REQUIRE(enum_lookup.Get("EnumName2") == enum_val_2);
        REQUIRE(enum_lookup.Get("EnumName3") == enum_val_3);
	}

    SECTION("Test overwriting enum value is working")
	{
        GASS::EnumLookup enum_lookup;
        enum_lookup.Add("EnumName", 5);

        enum_lookup.Add("EnumName", 10);
        
		REQUIRE(enum_lookup.Get("EnumName") == 10);
	}
}
