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

	if (_OPTIONS["platform"] == "x64") then
	configuration "Debug"
		targetname "GASSPluginPhysX_d"
		defines { "DEBUG" }
		flags { "Symbols" }
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"tinyxml_d",
			"tbb_debug",
			"PhysX3CommonCHECKED_x64",
			"PhysX3CHECKED_x64",
			"PhysX3CookingCHECKED_x64",
			"PhysX3CharacterKinematicCHECKED_x64",
			"PhysX3ExtensionsCHECKED",
			"PhysX3VehicleCHECKED",
			"RepX3CHECKED",
			"RepXUpgrader3CHECKED",
			"PhysXProfileSDKCHECKED",
			"PhysXVisualDebuggerSDKCHECKED",
			"PxTaskCHECKED"
		}
	libdirs 
	{
		"../lib/",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib",
		"$(PHYSX_HOME)/Lib/win64"
	}

	configuration "Release"
		targetname "GASSPluginPhysX"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"tinyxml",
			"tbb",
			"PhysX3Common_x64",
			"PhysX3_x64",
			"PhysX3Cooking_x64",
			"PhysX3CharacterKinematic_x64",
			"PhysX3Extensions",
			"PhysX3Vehicle",
			"RepX3",
			"RepXUpgrader3",
			"PhysXProfileSDK",
			"PhysXVisualDebuggerSDK",
			"PxTask"
		}
	libdirs 
	{
		"../lib/",
		"../dependencies/lib",
		"$(PHYSX_HOME)/lib/win64",
		"$(BOOST_HOME)/lib"
	}
	else 
end

	