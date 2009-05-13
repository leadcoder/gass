project "GASSPluginOgre"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../Source/Plugins/Ogre/**.cpp", "../Source/Plugins/Ogre/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	includedirs 
	{ 
		"../Source",
		"../Dependencies/ogre/OgreMain/include",
		"../Dependencies/ogre/Dependencies/include",
		"../Dependencies/ogre/PlugIns/OctreeSceneManager/include",
		"../Dependencies/tinyxml",
		"../Dependencies/boost"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../Dependencies",
		"../Dependencies/ogre/lib",
		"../Dependencies/Ogre/Dependencies/lib/$(ConfigurationName)",
		"../Dependencies/tinyxml/lib",
		"../Dependencies/boost/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../Source",
		"../Dependencies/include/OGRE",
		"../Dependencies/include/OctreeSceneManager",
		"../Dependencies/include/tinyxml",
		"../Dependencies/boost"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"../Dependencies/lib/",
		"../Dependencies/lib/OGRE"
	}

end


	configuration "Debug"
		targetname "GASSPluginOgre_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OGREMain_d",
			"Plugin_OctreeSceneManager_d",
			"tinyxmld"
		}

	configuration "Release"
		targetname "GASSPluginOgre"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"OGREMain",
			"Plugin_OctreeSceneManager",
			"tinyxml"
		}



