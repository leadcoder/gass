project "GASSSim"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../Source/Sim/**.cpp", "../Source/Sim/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	dofile "helpfunctions.lua"

	includedirs 
	{ 
		"../Source",
		"../Dependencies/tinyxml",
		"../Dependencies/boost",
		"../Dependencies/tbb/include"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../Dependencies",
		"../Dependencies/tinyxml/lib",
		"../Dependencies/boost/lib",
		"../Dependencies/tbb/ia32/" .. tbverdir .. "/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }


		links 
		{
			"GASSCore_d",
			"Winmm",
			"tinyxmld",
			"tbb_debug"

		}

else

	includedirs 
	{ 
		"../Source",
		"../Dependencies/tinyxml",
		"../Dependencies/boost",
		"../Dependencies/tbb/include"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"../Dependencies/lib",
		"../Dependencies/tinyxml/lib"
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



