project "SimTest"
	kind "ConsoleApp"
	language "C++"
	files { "../Source/Test/SimTest.cpp" }
	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	targetdir ( "../bin/$(ConfigurationName)" )

	includedirs 
	{ 
		"../Source", 		
--		"../Dependencies/tinyxml",
		"../Dependencies/boost" 
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION
	}



	configuration "Debug"
		targetname "SimTest_d"
		defines { "DEBUG" }
	 	flags { "Symbols"}
		links { "GASSCore_d","GASSSim_d"}
		postbuildcommands 
		{
			"copy ..\\lib\\" .. _ACTION .. "\\GASSCore_d.dll ..\\bin\\$(ConfigurationName)", 
			"copy ..\\lib\\" .. _ACTION .. "\\GASSSim_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\lib\\" .. _ACTION .. "\\GASSPluginOgre_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\lib\\" .. _ACTION .. "\\GASSPluginOIS_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\lib\\" .. _ACTION .. "\\GASSPluginODE_d.dll ..\\bin\\$(ConfigurationName)" 
		}
	
	
	
	configuration "Release"
		targetname "SimTest"
	 	defines { "NDEBUG" }
	 	flags { "Optimize"}
		links { "GASSCore","GASSSim" }
