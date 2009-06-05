project "GASSCore"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Core/**.cpp", "../source/Core/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	dofile "helpfunctions.lua"

	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"../dependencies/boost",
		"../dependencies/tbb/include"
	}

	libdirs 
	{ 
		"../dependencies/tinyxml/lib",
		"../dependencies/boost/lib",
		"../dependencies/tbb/ia32/" .. tbverdir .. "/lib"
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
		"../source",
		"../dependencies/include",
		"../dependencies/include/tinyxml",
		"../dependencies/include/boost"
	}

	libdirs 
	{ 
		"../dependencies/lib"
	}


	configuration "Debug"
		targetname "GASSCore_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links { "tinyxmld" }


	
	configuration "Release"
		targetname "GASSCore"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links { "tinyxml", "Winmm" }

     end


