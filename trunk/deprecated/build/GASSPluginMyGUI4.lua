project "GASSPluginMyGUI"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/MyGUI/**.cpp", "../source/Plugins/MyGUI/**.h" }

	targetdir ( "../lib/" )

	


if (os.is("windows")) then

	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"../dependencies/MyGUI/include/MyGUI",
		"$(OGRE_HOME)/OgreMain/include",
		"$(OGRE_HOME)/include/Ogre",
		"$(BOOST_HOME)"
	}
	
	libdirs 
	{ 
		"../lib/",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib",
		"$(OGRE_HOME)/lib/Release",
		"$(OGRE_HOME)/lib/Debug"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
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

end


	configuration "Debug"
		targetname "GASSPluginMyGUI_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"tinyxml_d",
			"MyGUI.OgrePlatform_d",
			"MyGUIEngine_d",
			"OgreMain_d"
		}

	configuration "Release"
		targetname "GASSPluginMyGUI"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"tinyxml",
			"MyGUI.OgrePlatform",
			"MyGUIEngine",
			"OgreMain"
		}



