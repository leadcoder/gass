project "SimDemo"
	kind "ConsoleApp"
	language "C++"
	files { "../Source/*.cpp", "../Source/*.h" }
	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
	targetdir ( "../../common/bin/$(ConfigurationName)" )

	includedirs 
	{ 
		"../../../source", 		
--		"../../../dependencies/tinyxml",
		"../../../dependencies/boost" 
	}

	libdirs 
	{ 
		"../../../lib/" .. _ACTION
	}



	configuration "Debug"
		targetname "SimDemo_d"
		defines { "DEBUG" }
	 	flags { "Symbols"}
		links { "GASSCore_d","GASSSim_d"}
		postbuildcommands 
		{
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSCore_d.dll ..\\..\\common\\bin\\$(ConfigurationName)", 
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSSim_d.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginOgre_d.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginOIS_d.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginODE_d.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginPagedGeometry_d.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginOpenAL_d.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginGame_d.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginBase_d.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,

			"postbuild_debug.bat" 
		}
	
	
	
	configuration "Release"
		targetname "SimDemo"
	 	defines { "NDEBUG" }
	 	flags { "Optimize"}
		links { "GASSCore","GASSSim" }
		postbuildcommands 
		{
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSCore.dll ..\\..\\common\\bin\\$(ConfigurationName)", 
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSSim.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginOgre.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginOIS.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginODE.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginPagedGeometry.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginOpenAL.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginGame.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginBase.dll ..\\..\\common\\bin\\$(ConfigurationName)" ,
			"postbuild_release.bat" 
		}
