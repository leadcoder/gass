project "GASSPluginOIS"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/OIS/**.cpp", "../source/Plugins/OIS/**.h" }

	targetdir ( "../lib/" .. _ACTION )


if (os.is("windows")) then

	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"../dependencies/ois/includes",
		"$(BOOST_PATH)"
	}
	
	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies/tinyxml/lib",
		"../dependencies/ois/lib",
		"$(BOOST_PATH)/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies/tinyxml/lib",
		"$(BOOST_PATH)/lib"
	}

end


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



