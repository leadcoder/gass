project "GASSPluginGame"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/Game/**.cpp", "../source/Plugins/Game/**.h" }

	targetdir ( "../lib/" )


	includedirs 
	{ 
		"../source/Plugins/Game",
		"../source",
		"../dependencies/tinyxml",
		"$(BOOST_HOME)"

	}

	libdirs 
	{ 
		"../lib/",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib"

	}

if (os.is("windows")) then

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
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
			"tinyxml_d",
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



