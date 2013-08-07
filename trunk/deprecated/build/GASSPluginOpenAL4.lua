project "GASSPluginOpenAL"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/OpenAL/**.cpp", "../source/Plugins/OpenAL/**.h" }

	targetdir ( "../lib/" )

	includedirs 
	{ 
		"../source",
		"../dependencies/OpenAL1.1/include",
		"../dependencies/tinyxml",
		"$(BOOST_HOME)"
	}

	dofile "helpfunctions.lua"

	libdirs 
	{ 
		"../lib/",
		"../dependencies/lib",
		"$(BOOST_HOME)/lib"
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
			"tinyxml_d",
			"OpenAL32",
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
			"tinyxml_d",
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


