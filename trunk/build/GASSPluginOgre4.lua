project "GASSPluginOgre"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/Ogre/**.cpp", "../source/Plugins/Ogre/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	includedirs 
	{ 
		"../source",
		"../dependencies/ogre/OgreMain/include",
		"../dependencies/ogre/dependencies/include",
		"../dependencies/ogre/PlugIns/OctreeSceneManager/include",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"../dependencies/ogre/lib",
		"../dependencies/Ogre/dependencies/lib/$(ConfigurationName)",
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
		"$(OGRE_PATH)/PlugIns/OctreeSceneManager/include",
		"$(OGRE_PATH)/build/include",		
		"../dependencies/tinyxml"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies/tinyxml/lib",
		"$(OGRE_PATH)/build/lib",
		"$(BOOST_PATH)/lib"

	}

end


	configuration "Debug"
		targetname "GASSPluginOgre_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
if (os.is("windows")) then
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OgreMain_d",
			"Plugin_OctreeSceneManager_d",
			"tinyxmld"
		}
else
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OgreMain",
			"tinyxmld"
		}
end

	configuration "Release"
		targetname "GASSPluginOgre"
		defines { "NDEBUG" }
		flags { "Optimize" }
if (os.is("windows")) then
		links 
		{
			"GASSCore",
			"GASSSim",
			"OgreMain",
			"Plugin_OctreeSceneManager",
			"tinyxml"
		}
else
		links 
		{
			"GASSCore",
			"GASSSim",
			"OgreMain",
			"tinyxml"
		}
end


