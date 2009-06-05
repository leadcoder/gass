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
		"../dependencies/ogre/OgreMain/include",
		"../dependencies/ogre/dependencies/include",
		"../dependencies/ogre/PlugIns/OctreeSceneManager/include",
		"../dependencies/tinyxml",
		"../dependencies/boost"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"../dependencies/ogre/lib",
		"../dependencies/Ogre/dependencies/lib/$(ConfigurationName)",
		"../dependencies/tinyxml/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../source",
		"../dependencies/include/OGRE",
		"../dependencies/include/OctreeSceneManager",
		"../dependencies/include/tinyxml",
		"../dependencies/include/boost"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies/lib/",
		"../dependencies/lib/OGRE"
	}

end


	configuration "Debug"
		targetname "GASSPluginOIS_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OIS_d",
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



