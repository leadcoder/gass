project "GASSPluginHydrax"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/Hydrax/**.cpp", "../source/Plugins/Hydrax/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	includedirs 
	{ 
		"../source",
		"$(OGRE_PATH)/include/OGRE",
		"$(OGRE_PATH)/OGREMain/include",
		"../dependencies/Hydrax-v0.5.1/Hydrax/src",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(OGRE_PATH)/lib/$(ConfigurationName)",
		"../dependencies/Hydrax-v0.5.1/Hydrax/bin/$(ConfigurationName)",
		"../dependencies/tinyxml/lib",
		"$(BOOST_PATH)/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../source",
		"$(BOOST_PATH)",
		"$(OGRE_PATH)",
		"..\dependencies\Hydrax-v0.5.1\Hydrax\bin\$(ConfigurationName)",
		"../dependencies/include/tinyxml"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies/lib/",
		"$(BOOST_PATH)/lib"
	}

end


	configuration "Debug"
		targetname "GASSPluginHydrax_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OGREMain_d",
			"tinyxmld",
			"Hydrax_d"
		}

	configuration "Release"
		targetname "GASSPluginHydrax"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"OGREMain",
			"tinyxml",
			"Hydrax"
		}



