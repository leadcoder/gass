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
		"$(OGRE_PATH)/OgreMain/include",
		"../dependencies/PagedGeometry/include",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(OGRE_PATH)/build/lib/$(ConfigurationName)",
		"../dependencies/PagedGeometry/lib",
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
		"../dependencies/include/OGRE",
		"../dependencies/PagedGeometry/include",
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
		targetname "GASSPluginPagedGeometry_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OGREMain_d",
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
			"OGREMain",
			"tinyxml",
			"PagedGeometry"
		}



