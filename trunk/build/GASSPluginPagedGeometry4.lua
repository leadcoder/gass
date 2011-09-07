project "GASSPluginPagedGeometry"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/PagedGeometry/**.cpp", "../source/Plugins/PagedGeometry/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	includedirs 
	{ 
		"../source",
		"$(OGRE_PATH)/include/OGRE",
		"$(OGRE_PATH)/OgreMain/include",
		"../dependencies/PagedGeometry-1.1.1/include",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(OGRE_PATH)/lib/$(ConfigurationName)",
		"../dependencies/PagedGeometry-1.1.1/lib",
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
		"$(OGRE_PATH)/include/OGRE",
		"$(OGRE_PATH)/OgreMain/include",
		"../dependencies/PagedGeometry-1.1.1/include",
		"../dependencies/include/tinyxml"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies/PagedGeometry-1.1.1/lib",
		"$(BOOST_PATH)/lib",
		"../dependencies/tinyxml/lib",
	}

end


	configuration "Debug"
		targetname "GASSPluginPagedGeometry_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OgreMain_d",
			"tinyxmld",
			"PagedGeometry_d"
		}

	configuration "Release"
		targetname "GASSPluginPagedGeometry"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"OgreMain",
			"tinyxml",
			"PagedGeometry"
		}



