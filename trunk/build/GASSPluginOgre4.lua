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
		"../dependencies/tinyxml",
		--support both source build and SDK
		--SDK
		"$(OGRE_PATH)/include/OGRE",
		"$(OGRE_PATH)/include/OGRE/Plugins/OctreeSceneManager",
		"$(OGRE_PATH)/include/OGRE/Terrain",
		"$(OGRE_PATH)/include/OGRE/Paging",
		--Same  inludes but form source
		"$(OGRE_PATH)/OGREMain/include",
		"$(OGRE_PATH)/PlugIns/OctreeSceneManager/include",
		"$(OGRE_PATH)/Components/Terrain/include",
		"$(OGRE_PATH)/Components/Paging/include",
		"$(BOOST_PATH)"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(OGRE_PATH)/lib/$(ConfigurationName)",
		"$(OGRE_PATH)/lib/$(ConfigurationName)/opt",
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
		"$(OGRE_PATH)/Components/Terrain/include",
		"$(OGRE_PATH)/Components/Paging/include",
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
			"OgreTerrain_d",
			"OgrePaging_d",
			"tinyxmld"
		}
else
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OgreMain",
			"OgreTerrain",
			"OgrePaging",
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
			"OgreTerrain",
			"OgrePaging",
			"tinyxml"
		}
else
		links 
		{
			"GASSCore",
			"GASSSim",
			"OgreMain",
			"OgreTerrain",
			"OgrePaging",
			"tinyxml"
		}
end


