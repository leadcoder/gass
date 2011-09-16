project "GASSPluginRakNet"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/RakNet/**.cpp", "../source/Plugins/RakNet/**.h" }

	targetdir ( "../lib/" .. _ACTION )
	dofile "helpfunctions.lua"

if (os.is("windows")) then
	includedirs 
	{ 
		"../source",
		"../dependencies/RakNet/Source",
		"../dependencies/tinyxml",
		"$(BOOST_HOME)",
		"../dependencies/tbb/include"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(BOOST_HOME)/lib",
		"../dependencies/tinyxml/lib",
		"../dependencies/RakNet/Lib",
		"../dependencies/tbb/lib/ia32/" .. tbverdir
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS", "dDOUBLE" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }

configuration "Debug"
		targetname "GASSPluginRakNet_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"RakNetLibStaticDebug",
			"tinyxmld",
			"tbb_debug",
			"ws2_32"
		}

	configuration "Release"
		targetname "GASSPluginRakNet"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"RakNetLibStatic",
			"tinyxml",
			"tbb",
			"ws2_32"
		}
	
else
	includedirs 
	{ 
		"../source",
		"../dependencies/RakNet/Source",
		"../dependencies/tinyxml",
		"$(BOOST_HOME)",
		"../dependencies/tbb/include"	
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(BOOST_HOME)/lib",
		"../dependencies/tinyxml/lib",
		"../dependencies/RakNet/Lib",
		"../dependencies/tbb/lib/ia32/" .. tbverdir 
	}
configuration "Debug"
		targetname "GASSPluginRakNet_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"RakNetLibStaticDebug",
			"tinyxmld",
			"tbb_debug"
		}

	configuration "Release"
		targetname "GASSPluginRakNet"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"RakNetLibStatic",
			"tinyxml",
			"tbb"
		}

end


