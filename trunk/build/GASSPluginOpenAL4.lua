project "GASSPluginOpenAL"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/OpenAL/**.cpp", "../source/Plugins/OpenAL/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then
	includedirs 
	{ 
		"../source",
		"../dependencies/OpenAL1.1/include",
		"../dependencies/tinyxml",
		"../dependencies/boost"
	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies",
		"../dependencies/ogre/lib",
		"../dependencies/OpenAL1.1/libs/Win32",
		"../dependencies/tinyxml/lib",
		"../dependencies/boost/lib"
	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../source",
		"../dependencies/include/boost",
		"../dependencies/OpenAL/include",
		"../dependencies/include/tinyxml"
	}

	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies/lib/",
		"../dependencies/lib/OpenAL1.1/libs/Linux",
	}

end


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



