project "CoreTest"
	kind "ConsoleApp"
	language "C++"
	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	targetdir ( "../bin/$(ConfigurationName)" )

	files { "../Source/Test/CoreTest.cpp" }

	includedirs 
	{ 
		"../Source",
		"../../Dependencies/boost" 
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../../Dependencies/boost/lib"
	}



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


