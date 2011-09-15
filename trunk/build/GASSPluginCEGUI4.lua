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
		"$(CEGUI_HOME)/CEGUI/include",
		"$(CEGUI_HOME)/CEGUI/include/ScriptingModules/LuaScriptModule/support/tolua++",
		"$(CEGUI_HOME)/dependencies/include",
		"$(OGRE_HOME)/OgreMain/include",
		"$(OGRE_HOME)/include/Ogre",
		"$(BOOST_HOME)"

	}
	
	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies/tinyxml/lib",
		"$(CEGUI_HOME)/lib",
		"$(CEGUI_HOME)/dependencies/lib/static",
		"$(BOOST_HOME)/lib"

	}

	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "GASS_PLUGIN_EXPORTS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	
else
	includedirs 
	{ 
		"../source",
		"../dependencies/tinyxml",
		"$(BOOST_HOME)"

	}

	libdirs 
	{ 
		"../lib/" .. _ACTION,
		"../dependencies/tinyxml/lib",
		"$(BOOST_HOME)/lib"

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
			"CEGUILuaScriptModule_d",
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



