project "GASSPluginHavok"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/Havok/**.cpp", "../source/Plugins/Havok/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS"}
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
end

	includedirs 
	{ 
		"../source",
		"$(HAVOK_HOME)/source",
		"../dependencies/tinyxml",
		"$(BOOST_HOME)",
		"../dependencies/tbb/include"
	}

	
 

	configuration "Debug"
		targetname "GASSPluginHavok_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"tinyxmld",
			"tbb_debug",
			"hkgBridge",
			"hkgCommon",
"hkpUtilities",
"hkpVehicle",
"hkpDynamics",
"hkpCollide",
"hkpInternal",
"hkpConstraintSolver",
"hkaRagdoll",
"hkaInternal",
"hkaAnimation",
"hkInternal",
"hkGeometryUtilities",
--hkVisualize",
"hkSerialize",
"hkCompat",
"hkSceneData",
"hkBase",
"hkcdInternal",
"hkcdCollide"
		}
	
	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(BOOST_HOME)/lib",
		"../dependencies/tinyxml/lib",
		"$(HAVOK_HOME)/Lib/win32_vs2010/Debug_Multithreaded_DLL",
		"../dependencies/tbb/lib/ia32/" .. tbverdir 

	}

	configuration "Release"
		targetname "GASSPluginHavok"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
	--		"ode_double",
			"tinyxml",
			"tbb" 
		}
libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(BOOST_HOME)/lib",
		"../dependencies/tinyxml/lib",
		"$(HAVOK_HOME)/Lib/win32_vs2010/Release_Multithreaded_DLL",
		"../dependencies/tbb/lib/ia32/" .. tbverdir 

	}



