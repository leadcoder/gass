project "GASSPluginAI"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/AI/**.cpp", "../source/Plugins/AI/**.h" }

	targetdir ( "../lib/" )

	dofile "helpfunctions.lua"

if (os.is("windows")) then

	includedirs 
	{ 
		"../source",
		"../source/Plugins/AI/Detour",
		"../source/Plugins/AI/DetourCrowd",
		"../source/Plugins/AI/Recast",
		"../dependencies/tinyxml",
		"$(BOOST_HOME)",
		"$(DXSDK_DIR)/include"
	}
	
	libdirs 
	{ 
		"../lib/",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }

configuration "Debug"
		targetname "GASSPluginAI_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"tinyxml_d"
		}

	configuration "Release"
		targetname "GASSPluginAI"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"tinyxml"
		}
	
else
	includedirs 
	{ 
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

configuration "Debug"
		targetname "GASSPluginAI_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"tinyxml_d"
		}

	configuration "Release"
		targetname "GASSPluginAI"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"tinyxml"
		}

end



