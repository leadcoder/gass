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
		"../dependencies/boost"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"../dependencies/tinyxml/lib",
		"../dependencies/boost/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../source/Plugins/Game",
		"../source",
		"../dependencies/include/boost",
			"../dependencies/include/tinyxml"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
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



