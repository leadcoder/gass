project "CoreTest"
	kind "ConsoleApp"
	language "C++"
	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	targetdir ( "../bin/$(ConfigurationName)" )

	files { "../source/Test/CoreUnitTest.cpp" }


if (os.is("windows")) then
	includedirs 
	{ 
		"../source",
		"$(BOOST_PATH)" 
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"$(BOOST_PATH)/lib"
	}

else
	includedirs 
	{ 
		"../source",
		"$(BOOST_PATH)/boost"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"$(BOOST_PATH)/lib"
	}
end

	configuration "Debug"
		targetname "CoreTest_d"
		defines { "DEBUG" }
	 	flags { "Symbols"}
		links { "GASSCore_d" }
		postbuildcommands 
		{
			"copy ..\\lib\\" .. _ACTION .. "\\GASSCore_d.dll ..\\bin\\$(ConfigurationName)" 
		}
	
	configuration "Release"
		targetname "CoreTest"
	 	defines { "NDEBUG" }
	 	flags { "Optimize"}
		links { "GASSCore" }


