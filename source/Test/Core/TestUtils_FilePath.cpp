#include "Core/Utils/GASSFilePath.h"

#include "catch.hpp"

TEST_CASE("Basic empty FilePath test")
{
	GASS::FilePath fp;

	SECTION("Test if FilePath is initialized with empty paths when using default constructor") {
		REQUIRE(fp.GetFullPath() == "");
		REQUIRE(fp.GetRawPath() == "");
	}

	SECTION("Test if GetLastFolder returns empty string") {
		REQUIRE(fp.GetFullPath() == "");
	}

	SECTION("Test if GetRawPath returns empty string") {
		REQUIRE(fp.GetRawPath() == "");
	}

	SECTION("Test if GetPathNoExtension returns empty string") {
		REQUIRE(fp.GetPathNoExtension() == "");
	}

	SECTION("Test if GetPathNoFile returns empty string") {
		REQUIRE(fp.GetPathNoFile() == "");
	}

	SECTION("Test if GetExtension returns empty string") {
		REQUIRE(fp.GetExtension() == "");
	}

	SECTION("Test if GetFilename returns empty string") {
		REQUIRE(fp.GetFilename() == "");
	}

	SECTION("Test if GetStem returns empty string") {
		REQUIRE(fp.GetStem() == "");
	}

	SECTION("Test if GetLastFolder returns empty string") {
		REQUIRE(fp.GetLastFolder() == "");
	}

	SECTION("Test if Exist returns false for empty string") {
		REQUIRE(!fp.Exist());
	}

	SECTION("Test if IsDir returns false for empty string") {
		REQUIRE(!fp.IsDir());
	}
}

TEST_CASE("Test SetPath FilePath test")
{
	GASS::FilePath fp();
	
}

