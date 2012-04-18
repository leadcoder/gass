project "GASSCore"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Core/**.cpp", "../source/Core/**.h" }

	targetdir ( "../lib/" )



	dofile "helpfunctions.lua"

	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"$(BOOST_HOME)",
		"../dependencies/tbb/include"
	}

	libdirs 
	{ 
		"../dependencies/lib",
		"$(BOOST_HOME)/lib"
	}


if (os.is("windows")) then
	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_CORE_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }

	configuration "Debug"
		targetname "GASSCore_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links { "tinyxml_d", "Winmm" }


	
	configuration "Release"
		targetname "GASSCore"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links { "tinyxml", "Winmm" }

else
	configuration "Debug"
		targetname "libGASSCore_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links { "tinyxml_d" }


	
	configuration "Release"
		targetname "libGASSCore"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links { "tinyxml" }

end

