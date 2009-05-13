project "GASSPluginODE"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../Source/Plugins/ODE/**.cpp", "../Source/Plugins/ODE/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	includedirs 
	{ 
		"../Source",
		"../Dependencies/ode/include",
		"../Dependencies/tinyxml",
		"../Dependencies/boost"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../Dependencies",
		"../Dependencies/tinyxml/lib",
		"../Dependencies/ode/lib/$(ConfigurationName)DoubleLib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS", "dDOUBLE" }
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
		targetname "GASSPluginODE_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"ode_doubled",
			"tinyxmld"
		}

	configuration "Release"
		targetname "GASSPluginODE"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"ode_double",
			"tinyxml"
		}



