project "GASSPluginOIS"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/OIS/**.cpp", "../source/Plugins/OIS/**.h" }

	targetdir ( "../lib/" .. _ACTION )

	dofile "helpfunctions.lua"

if (os.is("windows")) then

	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"../dependencies/ois/includes",
		"$(BOOST_HOME)",
		"$(DXSDK_DIR)/include"
	}
	
	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies/tinyxml/lib",
		"../dependencies/ois/lib",
		"$(BOOST_HOME)/lib",
		"$(DXSDK_DIR)/lib/x86"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }

configuration "Debug"
		targetname "GASSPluginOIS_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OIS_static_d",
			"tinyxmld"
		}

	configuration "Release"
		targetname "GASSPluginOIS"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"OIS_static",
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
		"../lib/" .. _ACTION,
		"../dependencies/tinyxml/lib",
		"$(BOOST_HOME)/lib"
	}

configuration "Debug"
		targetname "GASSPluginOIS_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OIS",
			"tinyxmld"
		}

	configuration "Release"
		targetname "GASSPluginOIS"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"OIS",
			"tinyxml"
		}

end



