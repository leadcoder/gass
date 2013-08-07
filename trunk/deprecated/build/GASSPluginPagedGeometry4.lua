project "GASSPluginPagedGeometry"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/PagedGeometry/**.cpp", "../source/Plugins/PagedGeometry/**.h" }

	targetdir ( "../lib/" )

if (os.is("windows")) then
	includedirs 
	{ 
		"../source",
		"$(OGRE_HOME)/include/OGRE",
		"$(OGRE_HOME)/OgreMain/include",
		"../dependencies/PagedGeometry-1.1.1/include",
		"../dependencies/tinyxml",
		"$(BOOST_HOME)"
	}

	libdirs 
	{ 
		"../lib/",
		"../dependencies/lib",
		"$(OGRE_HOME)/lib/$(ConfigurationName)",
		"$(BOOST_HOME)/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../source",
		"$(BOOST_HOME)",
		"$(OGRE_HOME)/include/OGRE",
		"$(OGRE_HOME)/OgreMain/include",
		"../dependencies/PagedGeometry-1.1.1/include",
		"../dependencies/include/tinyxml"
	}

	libdirs 
	{
		"../lib/",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib"
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
			"tinyxml_d",
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



