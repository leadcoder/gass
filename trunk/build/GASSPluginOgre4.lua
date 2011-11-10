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
		"../dependencies/tbb/include",
		--support both source build and SDK
		--SDK
		"$(OGRE_HOME)/include/OGRE",
		"$(OGRE_HOME)/include/OGRE/Plugins/OctreeSceneManager",
		"$(OGRE_HOME)/include/OGRE/Terrain",
		"$(OGRE_HOME)/include/OGRE/Paging",
		--Same  inludes but from source
		"$(OGRE_HOME)/OGREMain/include",
		"$(OGRE_HOME)/PlugIns/OctreeSceneManager/include",
		"$(OGRE_HOME)/Components/Terrain/include",
		"$(OGRE_HOME)/Components/Paging/include",
		"$(BOOST_HOME)"
	}
	
	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }

	configuration "Debug"
	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(OGRE_HOME)/lib\\debug",
		"$(OGRE_HOME)\\lib\\debug\\opt",
		"../dependencies/tinyxml/lib",
		"$(BOOST_HOME)/lib",
		"../dependencies/tbb/lib/" .. tbverdir 

	}
	
	
	configuration "Release"
	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies",
		"../dependencies/tbb/include",
		"$(OGRE_HOME)/lib/release",
		"$(OGRE_HOME)/lib/release/opt",
		"../dependencies/tinyxml/lib",
		"$(BOOST_HOME)/lib",
		"../dependencies/tbb/lib/" .. tbverdir
	}

	
	
else
	includedirs 
	{ 
		"../source",
		"$(BOOST_HOME)",
		"$(OGRE_HOME)/OgreMain/include",
		"$(OGRE_HOME)/PlugIns/OctreeSceneManager/include",
		"$(OGRE_HOME)/Components/Terrain/include",
		"$(OGRE_HOME)/Components/Paging/include",
		"$(OGRE_HOME)/build/include",		
		"../dependencies/tinyxml"
	}

	
	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies/tinyxml/lib",
		"$(OGRE_HOME)/build/lib",
		"$(BOOST_HOME)/lib"

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
			"tinyxmld",
			"tbb_debug"
		}
else
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OgreMain",
			"OgreTerrain",
			"OgrePaging",
			"tinyxmld",
			"tbb_debug"
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
			"tinyxml",
			"tbb"
		}
else
		links 
		{
			"GASSCore",
			"GASSSim",
			"OgreMain",
			"OgreTerrain",
			"OgrePaging",
			"tinyxml",
			"tbb"
		}
end


