project "GASSCore"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../Source/Core/**.cpp", "../Source/Core/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	dofile "helpfunctions.lua"

	includedirs 
	{ 
		"../Source",
		"../../Dependencies/tinyxml",
		"../../Dependencies/boost",
		"../../Dependencies/tbb/include"
	}

	libdirs 
	{ 
		"../../Dependencies/tinyxml/lib",
		"../../Dependencies/boost/lib",
		"../../Dependencies/tbb/ia32/" .. tbverdir .. "/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_CORE_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	targetdir ( "../lib/" .. _ACTION )
--	copy ..\lib\.. _ACTION .. "//GASSCore_d.dll ..\bin\Debug

	configuration "Debug"
		targetname "GASSCore_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links { "tinyxmld", "Winmm" }


	
	configuration "Release"
		targetname "GASSCore"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links { "tinyxml", "Winmm" }

     else

	includedirs 
	{ 
		"../Source",
		"../../Dependencies/include/tinyxml",
		"../../Dependencies/include/boost",
		"../../Dependencies/include"
	}

	libdirs 
	{ 
		"../../Dependencies/lib"
	}


	configuration "Debug"
		targetname "GASSCore_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links { "tinyxml" }


	
	configuration "Release"
		targetname "GASSCore"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links { "tinyxml", "Winmm" }

     end


