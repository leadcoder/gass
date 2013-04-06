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
		"$(BOOST_HOME)",
                "../../../dependencies/tbb/include"
	}

	libdirs 
	{ 
		"../../../lib/",
		"$(BOOST_HOME)/lib",
		"../../../dependencies/lib/"
	}

	configuration "Debug"
		targetname "SimDemo_d"
		defines { "DEBUG" }
	 	flags { "Symbols"}
		links { "GASSCore_d","GASSSim_d","tinyxml_d","tbb_debug"}
		targetdir ( "../bin/Debug" )

		postbuildcommands 
		{
			"copy ..\\..\\..\\lib\\GASSCore_d.dll ..\\bin\\$(ConfigurationName)", 
			"copy ..\\..\\..\\lib\\GASSSim_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginOgre_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginOIS_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginODE_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginPagedGeometry_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginEnvironment_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginOpenAL_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginGame_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginBase_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginRakNet_d.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\Dependencies\\lib\\tbb_debug.dll ..\\bin\\$(ConfigurationName)" ,
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
			"copy ..\\..\\..\\lib\\GASSCore.dll ..\\bin\\$(ConfigurationName)", 
			"copy ..\\..\\..\\lib\\GASSSim.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginOgre.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginOIS.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginODE.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginPagedGeometry.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginEnvironment.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginOpenAL.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginGame.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginBase.dll ..\\bin\\$(ConfigurationName)" ,
			"copy ..\\..\\..\\lib\\GASSPluginRakNet.dll ..\\bin\\$(ConfigurationName)" ,		
			"copy ..\\..\\..\\Dependencies\\lib\\tbb.dll ..\\bin\\$(ConfigurationName)" ,
			"postbuild_release.bat" 
		}
