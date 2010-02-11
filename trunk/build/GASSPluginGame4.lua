project "GASSPluginGame"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/Game/**.cpp", "../source/Plugins/Game/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	includedirs 
	{ 
		"../source/Plugins/Game",
		"../source",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)"

	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"../dependencies/tinyxml/lib",
		"$(BOOST_PATH)/lib"

	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../source/Plugins/Game",
		"../source",
		"$(BOOST_PATH)",
		"../dependencies/include/tinyxml"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"$(BOOST_PATH)/lib",
		"../dependencies/lib/",
	}

end


	configuration "Debug"
		targetname "GASSPluginGame_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"tinyxmld",
		}

	configuration "Release"
		targetname "GASSPluginGame"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"tinyxml",
		}



