project "GASSPluginOSG"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/OSG/**.cpp", "../source/Plugins/OSG/**.h" }

	targetdir ( "../lib/" )

if (os.is("windows")) then
	includedirs 
	{ 
		"../source",
		"$(OSG_HOME)/include",
		"../dependencies/tinyxml",
		"../dependencies/tbb/include",
		"$(BOOST_HOME)"
	}

	libdirs 
	{ 
		"../lib/",
		"$(OSG_HOME)/lib",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../source",
		"$(BOOST_HOME)",
		"$(OSG_HOME)/include",
		"../dependencies/include/tinyxml",
		"../dependencies/tbb/include"
	}

	libdirs 
	{
		"../lib/",
		"$(OSG_HOME)/lib",
		"../dependencies/lib/"
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
			"osgShadowd",
			"opengl32",
			"glu32",
			"tinyxml_d",
			"tbb_debug"  
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
			"osgShadow",
			"opengl32",
			"glu32",
			"tinyxml",
			"tbb"  
		}



