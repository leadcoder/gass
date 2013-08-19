project "GASSPluginScript"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/Script/**.cpp", "../source/Plugins/Script/**.cxx", "../source/Plugins/Script/**.h" }

	targetdir ( "../lib/" )


if (os.is("windows")) then

	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"../dependencies/lua/include",
		"$(BOOST_HOME)"

	}
	
	libdirs 
	{ 
		"../lib/",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"../dependencies/lua/include",
		"$(BOOST_HOME)"

	}

	libdirs 
	{ 
		"../lib/",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib"

	}

end


	configuration "Debug"
		targetname "GASSPluginScript_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"lua_d",
			"tinyxml_d"
		}

	configuration "Release"
		targetname "GASSPluginScript"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"lua",
			"tinyxml"
		}


