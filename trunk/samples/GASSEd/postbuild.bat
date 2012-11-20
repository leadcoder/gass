
cd bin
md Debug
cd Debug
md GASSPlugins
cd..
md Release
cd Release
md GASSPlugins
cd..
cd..

copy "%GASS_HOME%\\lib\\GASSCore_d.dll" bin\\debug
copy "%GASS_HOME%\\lib\\GASSSim_d.dll" bin\\debug
copy "%GASS_HOME%\\lib\\GASSEditorModule_d.dll" bin\\debug
copy "%GASS_HOME%\\lib\\GASSPluginOgre_d.dll" bin\\debug\\GASSPlugins
rem copy "%GASS_HOME%\\lib\\GASSPluginOSG_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginOIS_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginODE_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginGame_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginBase_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginEnvironment_d.dll" bin\\debug\\GASSPlugins
rem copy "%GASS_HOME%\\lib\\GASSPluginPagedGeometry_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginRaknet_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginOpenAL_d.dll" bin\\debug\\GASSPlugins



copy "%GASS_HOME%\\lib\\GASSCore.dll" bin\\release
copy "%GASS_HOME%\\lib\\GASSSim.dll" bin\\release
copy "%GASS_HOME%\\lib\\GASSEditorModule.dll" bin\\release
copy "%GASS_HOME%\\lib\\GASSPluginOgre.dll" bin\\release\\GASSPlugins
rem copy "%GASS_HOME%\\lib\\GASSPluginOSG.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginOIS.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginODE.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginGame.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginBase.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginEnvironment.dll" bin\\release\\GASSPlugins
rem copy "%GASS_HOME%\\lib\\GASSPluginPagedGeometry.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginRaknet.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\lib\\GASSPluginOpenAL.dll" bin\\release\\GASSPlugins

copy "%GASS_HOME%\Dependencies\lib\tbb_debug.dll" bin\\Debug
copy "%GASS_HOME%\Dependencies\lib\tbb.dll" bin\\Release


copy "%OGRE_HOME%\bin\debug\*_d.dll" bin\\Debug\
copy "%OGRE_HOME%\bin\debug\cg.dll" bin\\Debug\

copy "%OGRE_HOME%\bin\release\*.dll" bin\\Release
copy "%OGRE_HOME%\bin\debug\cg.dll" bin\\Release

copy "%GASS_HOME%\Dependencies\lib\ois_d.dll" bin\\Debug
copy "%GASS_HOME%\Dependencies\lib\ois.dll" bin\\Release

copy "%GASS_HOME%\Dependencies\OpenAL1.1\libs\Win32\OpenAL32.dll" bin\\Debug
copy "%GASS_HOME%\Dependencies\OpenAL1.1\libs\Win32\wrap_oal.dll" bin\\Debug

copy "%GASS_HOME%\Dependencies\OpenAL1.1\libs\Win32\OpenAL32.dll" bin\\Release
copy "%GASS_HOME%\Dependencies\OpenAL1.1\libs\Win32\wrap_oal.dll" bin\\Release


copy "%QTDIR%\Dependencies\OpenAL1.1\libs\Win32\OpenAL32.dll" bin\\Release



pause



