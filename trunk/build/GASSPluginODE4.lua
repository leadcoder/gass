project "GASSPluginODE"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/ODE/**.cpp", "../source/Plugins/ODE/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	includedirs 
	{ 
		"../Source",
		"../dependencies/ode/include",
		"../dependencies/tinyxml",
		"../dependencies/boost",
		"../dependencies/tbb/include"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"../dependencies/tinyxml/lib",
		"../dependencies/ode/lib/$(ConfigurationName)DoubleLib",
		"../dependencies/tbb/ia32/" .. tbverdir .. "/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS", "dDOUBLE" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../source",
		"../dependencies/include",
		"../dependencies/include/boost",
--		"../dependencies/include/OGRE",
--		"../dependencies/include/OctreeSceneManager",
		"../dependencies/include/tinyxml"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies/lib/",
		"../dependencies/lib/OGRE"
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
			"tinyxmld",
			"tbb_debug"  
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
			"tinyxml",
			"tbb" 
		}



