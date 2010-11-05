project "GASSPluginCEGUI"
	kind "SharedLib"
	language "C++"
	targetprefix "" -- exchange this with soname in some way?

	files { "../source/Plugins/CEGUI/**.cpp", "../source/Plugins/CEGUI/**.h" }

	targetdir ( "../lib/" .. _ACTION )

	


if (os.is("windows")) then

	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"../dependencies/CEGUI-SDK-0.7.2/CEGUI/include",
		"$(OGRE_PATH)/OgreMain/include",
		"$(BOOST_PATH)"

	}
	
	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies/tinyxml/lib",
		"../dependencies/CEGUI-SDK-0.7.2/lib",
		"$(BOOST_PATH)/lib"

	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"$(BOOST_PATH)"

	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies/tinyxml/lib",
		"$(BOOST_PATH)/lib"

	}

end


	configuration "Debug"
		targetname "GASSPluginCEGUI_d"
		defines { "DEBUG" }
		flags { "Symbols" }
--		debugPrefix = "_d"
		links 
		{
			"GASSCore_d",
			"GASSSim_d",
			"CEGUIBase_d",
			"CEGUIOgreRenderer_d",
			"CEGUILuaScriptModule_d.lib",
			"tinyxmld"
		}

	configuration "Release"
		targetname "GASSPluginCEGUI"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links 
		{
			"GASSCore",
			"GASSSim",
			"CEGUIBase",
			"CEGUIOgreRenderer",
			"CEGUILuaScriptModule",
			"tinyxml"
		}



