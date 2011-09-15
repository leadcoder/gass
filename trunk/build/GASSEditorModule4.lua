project "GASSEditorModule"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Modules/Editor/**.cpp", "../source/Modules/Editor/**.h" }

	targetdir ( "../lib/" .. _ACTION )

if (os.is("windows")) then

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS","EDITOR_MODULE_EXPORTS"}
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
       defines {"EDITOR_MODULE_EXPORTS" }

end

	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"$(BOOST_HOME)",
		"../dependencies/tbb/include"
	}

	configuration "Debug"
		targetname "GASSEditorModule_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"tinyxmld",
			"tbb_debug"  
		}
	libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(BOOST_HOME)/lib",
		"../dependencies/tinyxml/lib",
		"../dependencies/tbb/lib/ia32/" .. tbverdir 

	}

	configuration "Release"
		targetname "GASSEditorModule"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"tinyxml",
			"tbb" 
		}
libdirs 
	{
		"../lib/" .. _ACTION,
		"../dependencies",
		"$(BOOST_HOME)/lib",
		"../dependencies/tinyxml/lib",
		"../dependencies/tbb/lib/ia32/" .. tbverdir 
	}



