rem ECHO Copy GASS dll files
IF "%1" == "Debug" (
	::copying 
 	copy "%2\debug\GASSCore_d.dll" "%3"
	copy "%2\debug\GASSSim_d.dll" "%3"
	copy "%2\debug\GASSPluginOgre_d.dll" "%3"
	copy "%2\debug\GASSPluginOIS_d.dll" "%3"
	copy "%2\debug\GASSPluginODE_d.dll" "%3"
	copy "%2\debug\GASSPluginPagedGeometry_d.dll" "%3"
	copy "%2\debug\GASSPluginEnvironment_d.dll" "%3"
	copy "%2\debug\GASSPluginOpenAL_d.dll" "%3"
	copy "%2\debug\GASSPluginGame_d.dll" "%3"
	copy "%2\debug\GASSPluginBase_d.dll" "%3"
	copy "%2\debug\GASSPluginRakNet_d.dll" "%3"
)
IF "%1" == "Release" (
	::copying 
 	copy "%2\release\GASSCore.dll" "%3"
	copy "%2\release\GASSSim.dll" "%3"
	copy "%2\release\GASSPluginOgre.dll" "%3"
	copy "%2\release\GASSPluginOIS.dll" "%3"
	copy "%2\release\GASSPluginODE.dll" "%3"
	copy "%2\release\GASSPluginPagedGeometry.dll" "%3"
	copy "%2\release\GASSPluginEnvironment.dll" "%3"
	copy "%2\release\GASSPluginOpenAL.dll" "%3"
	copy "%2\release\GASSPluginGame.dll" "%3"
	copy "%2\release\GASSPluginBase.dll" "%3"
	copy "%2\release\GASSPluginRakNet.dll" "%3"
)

pause
