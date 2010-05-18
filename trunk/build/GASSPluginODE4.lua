project "GASSPluginODE"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/ODE/**.cpp", "../source/Plugins/ODE/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS", "dDOUBLE" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
       defines { "dDOUBLE" }

end

	includedirs 
	{ 
		"../source",
		"../dependencies/ode/include",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)",
		"../dependencies/tbb/include"
	}

	
 

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
	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(BOOST_PATH)/lib",
		"../dependencies/tinyxml/lib",
		"../dependencies/ode/lib/DebugDoubleLib",
		"../dependencies/tbb/ia32/" .. tbverdir .. "/lib"

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
libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(BOOST_PATH)/lib",
		"../dependencies/tinyxml/lib",
		"../dependencies/ode/lib/ReleaseDoubleLib",
		"../dependencies/tbb/ia32/" .. tbverdir .. "/lib"

	}



