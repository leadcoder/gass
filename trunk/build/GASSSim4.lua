project "GASSSim"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Sim/**.cpp", "../source/Sim/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
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

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }


else

	includedirs 
	{ 
		"../source",
		"../dependencies/include/tinyxml",
		"$(BOOST_PATH)",
		"../dependencies/include"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies/lib",
		"$(BOOST_PATH)/lib"
	}

end

	configuration "Debug"
		targetname "GASSSim_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links 
		{
		   "GASSCore_d",
		   "tinyxml",
		   "tbb_debug"  
		}


	configuration "Release"
		targetname "GASSSim"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"Winmm",
			"tinyxml",
			"tbb"
		}



