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
		"$(BOOST_PATH)",
		"../dependencies/tbb/include"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(BOOST_PATH)/lib",
		"../dependencies/tinyxml/lib",
		"../dependencies/ode/lib/$(ConfigurationName)DoubleLib",
		"../dependencies/tbb/ia32/" .. tbverdir .. "/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS", "dDOUBLE" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../Source",
		"../dependencies/ode/include",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)",
		"../dependencies/tbb/include"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(BOOST_PATH)/lib",
		"../dependencies/tinyxml/lib",
		"../dependencies/ode/lib/$(ConfigurationName)DoubleLib",
		"../dependencies/tbb/ia32/cc4.1.0_libc2.4_kernel2.6.16.21/lib"

	}
        defines { "dDOUBLE" }

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



