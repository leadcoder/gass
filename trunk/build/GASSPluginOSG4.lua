project "GASSPluginOSG"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/OSG/**.cpp", "../source/Plugins/OSG/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	includedirs 
	{ 
		"../source",
		"$(OSG_PATH)/include",
		"$(OSG_PATH)/build/include",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(OSG_PATH)/lib",
		"$(OSG_PATH)/build/lib",
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
		"$(OSG_PATH)/include",
		"$(OSG_PATH)/build/include",
		"../dependencies/include/tinyxml"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"$(OSG_PATH)/lib",
		"$(OSG_PATH)/build/lib"

	}

end


	configuration "Debug"
		targetname "GASSPluginOSG_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"OpenThreadsd",
			"osgd",
			"osgDBd",
			"osgUtild",
			"osgGAd",
			"osgViewerd",
			"osgTextd",
			"opengl32",
			"glu32",
			"tinyxmld"
		}

	configuration "Release"
		targetname "GASSPluginOSG"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"OpenThreads",
			"osg",
			"osgDB",
			"osgUtil",
			"osgGA",
			"osgViewer",
			"osgText",
			"opengl32",
			"glu32",
			"tinyxml"
		}



