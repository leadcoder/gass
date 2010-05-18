project "GASSSim"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Sim/**.cpp", "../source/Sim/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
end

	dofile "helpfunctions.lua"

	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)",
		"../dependencies/tbb/include"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"../dependencies/tinyxml/lib",
		"$(BOOST_PATH)/lib",
		"../dependencies/tbb/ia32/" .. tbverdir .. "/lib"
	}



	configuration "Debug"
		targetname "GASSSim_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links 
		{
		   "GASSCore_d",
		   "tinyxmld",
		   "tbb_debug"  
		}


	configuration "Release"
		targetname "GASSSim"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"tinyxml",
			"tbb"
		}



