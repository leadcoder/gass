project "CoreTest"
	kind "ConsoleApp"
	language "C++"
	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	targetdir ( "../bin/$(ConfigurationName)" )

	files { "../source/Test/CoreUnitTest.cpp" }


	includedirs 
	{ 
		"../source",
		"$(BOOST_PATH)" ,
		"../dependencies/tinyxml",


	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"$(BOOST_PATH)/lib",
		"../dependencies/tinyxml/lib",
	}

	configuration "Debug"
		targetname "CoreTest_d"
		defines { "DEBUG" }
	 	flags { "Symbols"}
		links { "GASSCore_d",  "tinyxml" }
		postbuildcommands 
		{
			"copy ..\\lib\\" .. _ACTION .. "\\GASSCore_d.dll ..\\bin\\$(ConfigurationName)" 
		}
	
	configuration "Release"
		targetname "CoreTest"
	 	defines { "NDEBUG" }
	 	flags { "Optimize"}
		links { "GASSCore", "tinyxml" }


