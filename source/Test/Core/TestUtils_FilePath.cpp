#include "Core/Utils/GASSFilePath.h"

#include "catch.hpp"

TEST_CASE("Basic empty FilePath test", "[!hide][Filesystem]")
{
	GASS::FilePath fp;

	SECTION("Test if FilePath is initialized with empty paths when using default constructor")
	{
		REQUIRE(fp.GetFullPath() == "");
		REQUIRE(fp.GetRawPath() == "");
	}

	SECTION("Test if GetLastFolder returns empty string")
	{
		REQUIRE(fp.GetFullPath() == "");
	}

	SECTION("Test if GetRawPath returns empty string")
	{
		REQUIRE(fp.GetRawPath() == "");
	}

	SECTION("Test if GetPathNoExtension returns empty string")
	{
		REQUIRE(fp.GetPathNoExtension() == "");
	}

	SECTION("Test if GetPathNoFile returns empty string")
	{
		REQUIRE(fp.GetPathNoFile() == "");
	}

	SECTION("Test if GetExtension returns empty string")
	{
		REQUIRE(fp.GetExtension() == "");
	}

	SECTION("Test if GetFilename returns empty string")
	{
		REQUIRE(fp.GetFilename() == "");
	}

	SECTION("Test if GetStem returns empty string")
	{
		REQUIRE(fp.GetStem() == "");
	}

	SECTION("Test if GetLastFolder returns empty string")
	{
		REQUIRE(fp.GetLastFolder() == "");
	}

	SECTION("Test if Exist returns false for empty string")
	{
		REQUIRE(!fp.Exist());
	}

	SECTION("Test if IsDir returns false for empty string")
	{
		REQUIRE(!fp.IsDir());
	}
}


TEST_CASE("Test SetPath FilePath test with absolute (win) path")
{
	GASS::FilePath abs_fp;
	abs_fp.SetPath("C:\\testing\\multiple\\folders");

	SECTION("Test path are set correctly with SetPath")
	{
		REQUIRE(abs_fp.GetRawPath() == abs_fp.GetFullPath());
	}

	SECTION("Test path are set correctly with SetPath")
	{
		REQUIRE(abs_fp.GetRawPath() == "C:/testing/multiple/folders");
	}
}

TEST_CASE("Test SetPath FilePath test with Windows path")
{
	GASS::FilePath rel_fp;
	rel_fp.SetPath("\\testing\\multiple\\folders");

	SECTION("Test path are set correctly with SetPath")
	{
		REQUIRE(rel_fp.GetRawPath() == rel_fp.GetFullPath());
	}

	SECTION("Test path are set correctly with SetPath")
	{
		REQUIRE(rel_fp.GetRawPath() == "/testing/multiple/folders");
	}
}


TEST_CASE("Test FilePath helper methods with valid file path")
{
	GASS::FilePath fp;
	fp.SetPath("C:\\Temp\\GASS\\file.txt");

	SECTION("Test GetPathNoExtension with ordinary file")
	{
		REQUIRE(fp.GetPathNoExtension() == "C:/Temp/GASS/file");
	}

	SECTION("Test GetPathNoExtension with file not including extension")
	{
		fp.SetPath("C:\\Temp\\GASS\\file");
		REQUIRE(fp.GetPathNoExtension() == "C:/Temp/GASS/file");
	}

	SECTION("Test GetPathNoExtension with ordinary file")
	{
		REQUIRE(fp.GetPathNoFile() == "C:/Temp/GASS/");
	}
}
/*TEST_CASE("Test FilePath sanitation when passing different number of slash/backslash")
{
	GASS::FilePath fp;
	fp.SetPath("C:\\Temp\\\\GASS\\\\file.txt");

	SECTION("Test various number of slas/backslash separating folders")
	{
		REQUIRE(fp.GetFullPath() == "C:/Temp/GASS/file.txt");
		
		fp.SetPath("C://///Temp///GASS/file.txt");
		REQUIRE(fp.GetFullPath() == "C:/Temp/GASS/file.txt");
	}
}*/