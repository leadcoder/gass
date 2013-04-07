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
			"PhysX3CommonDEBUG_x64",
			"PhysX3DEBUG_x64",
			"PhysX3CookingDEBUG_x64",
			"PhysX3CharacterKinematicDEBUG_x64",
			"PhysX3ExtensionsDEBUG",
			"PhysX3VehicleDEBUG",
			"RepX3DEBUG",
			"RepXUpgrader3DEBUG",
			"PhysXProfileSDKDEBUG",
			"PhysXVisualDebuggerSDKDEBUG",
			"PxTaskDEBUG"
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
			"PhysX3CommonDEBUG_x86",
			"PhysX3DEBUG_x86",
			"PhysX3CookingDEBUG_x86",
			"PhysX3CharacterKinematicDEBUG_x86",
			"PhysX3ExtensionsDEBUG",
			"PhysX3VehicleDEBUG",
			"RepX3DEBUG",
			"RepXUpgrader3DEBUG",
			"PhysXProfileSDKDEBUG",
			"PhysXVisualDebuggerSDKDEBUG",
			"PxTaskDEBUG"
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
			"tinyxml",
			"tbb",
			"PhysX3Common_x86",
			"PhysX3_x86",
			"PhysX3Cooking_x86",
			"PhysX3CharacterKinematic_x86",
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
		"$(PHYSX_HOME)/lib/win32",
		"$(BOOST_HOME)/lib"
	}
end

	