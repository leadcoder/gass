project "GASSPluginEnvironment"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/Environment/**.cpp", "../source/Plugins/Environment/**.h" }

	targetdir ( "../lib/" )

if (os.is("windows")) then
	includedirs 
	{ 
		"../source",
		"$(OGRE_HOME)/include/OGRE",
		"$(OGRE_HOME)/OgreMain/include",
		"../dependencies/Hydrax-v0.5.1/src",
		"../dependencies/SkyX-v0.1",
		"../dependencies/tinyxml",
		"$(BOOST_HOME)"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }

	configuration "Release"
	libdirs 
	{ 
		"../lib/",
		"../dependencies",
		"$(OGRE_HOME)/lib/Release",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib"
	}
	

	configuration "Debug"
	libdirs 
	{ 
		"../lib/",
		"../dependencies",
		"$(OGRE_HOME)/lib/Debug",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib"
	}
	
else
	includedirs 
	{ 
		"../source",
		"$(BOOST_HOME)",
		"$(OGRE_HOME)/OgreMain/include",
		"../dependencies/Hydrax-v0.5.1/src",
		"../dependencies/SkyX-v0.1",
		"../dependencies/include/tinyxml"
	}

	libdirs 
	{
		"../lib/",
		"$(OGRE_HOME)/build/lib",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib"
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
			"tinyxml_d",
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



