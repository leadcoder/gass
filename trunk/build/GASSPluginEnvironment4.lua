project "GASSPluginEnvironment"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/Environment/**.cpp", "../source/Plugins/Environment/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	includedirs 
	{ 
		"../source",
		"$(OGRE_PATH)/include/OGRE",
		"$(OGRE_PATH)/OgreMain/include",
		"../dependencies/Hydrax-v0.5.1/Hydrax/src",
		"../dependencies/SkyX-v0.1/SkyX",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(OGRE_PATH)/lib/$(ConfigurationName)",
		"../dependencies/Hydrax-v0.5.1/Hydrax/bin/$(ConfigurationName)",
		"../dependencies/SkyX-v0.1/SkyX/SkyX/bin",
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
		"$(OGRE_PATH)/OgreMain/include",
		"../dependencies/Hydrax-v0.5.1/Hydrax/src",
		"../dependencies/SkyX-v0.1/SkyX",
		"../dependencies/include/tinyxml"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"$(OGRE_PATH)/build/lib",
		"../dependencies/Hydrax-v0.5.1/Hydrax/bin",
		"../dependencies/SkyX-v0.1/SkyX/SkyX/bin",
		"../dependencies/tinyxml/lib",
		"$(BOOST_PATH)/lib"
	}

end


	configuration "Debug"
		targetname "GASSPluginEnvironment_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OgreMain_d",
			"tinyxmld",
			"Hydrax_d",
			"SkyX_d"

		}

	configuration "Release"
		targetname "GASSPluginEnvironment"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"OgreMain",
			"tinyxml",
			"Hydrax",
			"SkyX"
		}



