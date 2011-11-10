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
		"$(BOOST_HOME)",
		"../dependencies/tbb/include"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"../dependencies/tinyxml/lib",
		"$(BOOST_HOME)/lib",
		"../dependencies/tbb/lib/" .. tbverdir
	}


if (os.is("windows")) then
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
else
	configuration "Debug"
		targetname "libGASSSim_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links 
		{
		   "GASSCore_d",
		   "tinyxmld",
		   "tbb_debug"  
		}


	configuration "Release"
		targetname "libGASSSim"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"tinyxml",
			"tbb"
		}

end


