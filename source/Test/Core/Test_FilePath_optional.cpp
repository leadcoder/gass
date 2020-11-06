#include "Core/Utils/GASSFilePath.h"

#include "catch.hpp"


TEST_CASE("Test FilePath with windows share (UNC). Slow to run.", "[!hide][File system]")
{
	GASS::FilePath unc_fp("\\\\hostname\\test\\");
	
	SECTION("Test path are set correctly with SetPath")
	{
		REQUIRE(unc_fp.GetRawPath() == unc_fp.GetFullPath());
	}

	SECTION("Test path are set correctly with SetPath")
	{
		REQUIRE(unc_fp.GetRawPath() == "\\\\hostname/test/");
	}
}
