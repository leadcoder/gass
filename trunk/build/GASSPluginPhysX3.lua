project "GASSPluginPhysX3"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/PhysX3/**.cpp", "../source/Plugins/PhysX3/**.h" }

	targetdir ( "../lib/" )

if (os.is("windows")) then

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
       defines { "dDOUBLE" }
end

	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"$(BOOST_HOME)",
		"../dependencies/tbb/include",
		"$(PHYSX_HOME)/include"
	}

	configuration "Debug"
		targetname "GASSPluginPhysX_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"tinyxml_d",
			"tbb_debug"  ,
			"Foundation",
			"PhysX3Common",
"PhysX3CharacterDynamic",
"PhysX3CharacterKinematic",
"PhysX3Vehicle",
"PhysX3Cooking",
"PhysX3",
"PhysX3Extensions",
"LowLevel",
"GeomUtils",
"SceneQuery",
"SimulationController",
"PvdRuntime",
"RepX3"

		}
	libdirs 
	{
		"../lib/",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib",
		"$(PHYSX_HOME)/Lib/win32"
	}

	configuration "Release"
		targetname "GASSPluginPhysX"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"ODE",
			"tinyxml",
			"tbb",
			"PhysX3CharacterDynamic",
"PhysX3CharacterKinematic",
"PhysX3Vehicle",
"PhysX3Cooking",
"PhysX3",
"PhysX3Extensions",
"LowLevel",
"GeomUtils",
"SceneQuery",
"SimulationController",
"PvdRuntime",
"RepX3"

		}
libdirs 
	{
		"../lib/",
		"$(PHYSX_HOME)/SDKs/lib/win64",
		"$(BOOST_HOME)/lib"
	}
	