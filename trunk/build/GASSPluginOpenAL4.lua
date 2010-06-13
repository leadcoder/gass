project "GASSPluginOpenAL"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/OpenAL/**.cpp", "../source/Plugins/OpenAL/**.h" }

	targetdir ( "../lib/" .. _ACTION )

	includedirs 
	{ 
		"../source",
		"../dependencies/OpenAL1.1/include",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"../dependencies/ogre/lib",
		"../dependencies/OpenAL1.1/libs/Win32",
		"../dependencies/tinyxml/lib",
		"$(BOOST_PATH)/lib"
	}

if (os.is("windows")) then

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	



	configuration "Debug"
		targetname "GASSPluginOpenAL_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"tinyxmld",
			"OpenAL32",
			"ALut",
		}

	configuration "Release"
		targetname "GASSPluginOpenAL"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"tinyxml",
			"OpenAL32",
			"ALut",
		}
else 
	configuration "Debug"
		targetname "GASSPluginOpenAL_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"tinyxmld",
			"openal",
			"alut",
		}

	configuration "Release"
		targetname "GASSPluginOpenAL"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"tinyxml",
			"openal",
			"alut",
		}
end


