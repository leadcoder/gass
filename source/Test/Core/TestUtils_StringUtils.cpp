#include "Core/Utils/GASSStringUtils.h"

#include "catch.hpp"

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
}