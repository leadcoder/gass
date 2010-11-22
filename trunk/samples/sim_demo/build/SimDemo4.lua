project "SimDemo"
	kind "ConsoleApp"
	language "C++"
	files { "../source/*.cpp", "../source/*.h" }
if (os.is("windows")) then
	defines { "WIN32", "_CRT_SECURE_NO_WARNINGS" }
	flags { "NoPCH", "No64BitChecks" } --, "NoRTTI" }
end
	
        dofile "../../../build/helpfunctions.lua"

	includedirs 
	{ 
		"../../../source", 		
		"../../../dependencies/tinyxml",
		"$(BOOST_PATH)",
                "../../../dependencies/tbb/include"
	}

	libdirs 
	{ 
		"../../../lib/" .. _ACTION,
		"../../../dependencies/tinyxml/lib",
		"../../../dependencies/tbb/ia32/" .. tbverdir .. "/lib"
	}

	configuration "Debug"
		targetname "SimDemo_d"
		defines { "DEBUG" }
	 	flags { "Symbols"}
		links { "GASSCore_d","GASSSim_d","tinyxmld","tbb_debug"}
		targetdir ( "../bin/Debug" )

		postbuildcommands 
		{
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSCore_d.dll ..\\bin\\$(ConfigurationName)", 
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSSim_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginOgre_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginOIS_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginODE_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginPagedGeometry_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginOpenAL_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginGame_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginBase_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginRakNet_d.dll ..\\bin\\$(ConfigurationName)" ,

			"postbuild_debug.bat" 
		}
	
	
	
	configuration "Release"
		targetname "SimDemo"
	 	defines { "NDEBUG" }
	 	flags { "Optimize"}
		links { "GASSCore","GASSSim","tinyxml","tbb" }
                targetdir ( "../bin/Release" )
		postbuildcommands 
		{
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSCore.dll ..\\bin\\$(ConfigurationName)", 
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSSim.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginOgre.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginOIS.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginODE.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginPagedGeometry.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginOpenAL.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginGame.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginBase.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\" .. _ACTION .. "\\GASSPluginRakNet.dll ..\\bin\\$(ConfigurationName)" ,
			"postbuild_release.bat" 
		}
