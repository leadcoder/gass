
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

copy "%GASS_HOME%\\bin\\debug\\GASSCore_d.dll" bin\\debug
copy "%GASS_HOME%\\bin\\debug\\GASSSim_d.dll" bin\\debug
copy "%GASS_HOME%\\bin\\debug\\GASSEditorModule_d.dll" bin\\debug
copy "%GASS_HOME%\\bin\\debug\\plugins\\GASSPluginOgre_d.dll" bin\\debug\\GASSPlugins
rem copy "%GASS_HOME%\\bin\\GASSPluginOSG_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\plugins\\GASSPluginOIS_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\plugins\\GASSPluginODE_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\plugins\\GASSPluginGame_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\plugins\\GASSPluginBase_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\plugins\\GASSPluginEnvironment_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\plugins\\GASSPluginPagedGeometry_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\plugins\\GASSPluginRaknet_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\plugins\\GASSPluginOpenAL_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\plugins\\GASSPluginAI_d.dll" bin\\debug\\GASSPlugins


copy "%GASS_HOME%\\bin\\release\\GASSCore.dll" bin\\release
copy "%GASS_HOME%\\bin\\release\\GASSSim.dll" bin\\release
copy "%GASS_HOME%\\bin\\release\\GASSEditorModule.dll" bin\\release
copy "%GASS_HOME%\\bin\\release\\plugins\\GASSPluginOgre.dll" bin\\release\\GASSPlugins
rem copy "%GASS_HOME%\\bin\\GASSPluginOSG.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\plugins\\GASSPluginOIS.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\plugins\\GASSPluginODE.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\plugins\\GASSPluginGame.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\plugins\\GASSPluginBase.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\plugins\\GASSPluginEnvironment.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\plugins\\GASSPluginPagedGeometry.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\plugins\\GASSPluginRaknet.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\plugins\\GASSPluginOpenAL.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\plugins\\GASSPluginAI.dll" bin\\release\\GASSPlugins

copy "%GASS_DEPENDENCIES%\lib\tbb_debug.dll" bin\\Debug
copy "%GASS_DEPENDENCIES%\lib\tbb.dll" bin\\Release

copy "%OGRE_HOME%\bin\debug\*_d.dll" bin\\Debug\
copy "%OGRE_HOME%\bin\debug\cg.dll" bin\\Debug\

copy "%OGRE_HOME%\bin\release\*.dll" bin\\Release
copy "%OGRE_HOME%\bin\debug\cg.dll" bin\\Release

copy "%GASS_DEPENDENCIES%\lib\ois_d.dll" bin\\Debug
copy "%GASS_DEPENDENCIES%\lib\ois.dll" bin\\Release

copy "%GASS_DEPENDENCIES%\lib\OpenAL32.dll" bin\\Debug
copy "%GASS_DEPENDENCIES%\lib\OpenAL32.dll" bin\\Release


copy "%QT_HOME%\bin\QtCore4.dll" bin\\Release
copy "%QT_HOME%\bin\QtGui4.dll" bin\\Release

copy "%QT_HOME%\bin\QtCored4.dll" bin\\Debug
copy "%QT_HOME%\bin\QtGuid4.dll" bin\\Debug

copy  "%GASS_DEPENDENCIES%\qtpropertybrowser-2.5_1\lib\QtSolutions_PropertyBrowser-2.5.dll" bin\\Release
copy  "%GASS_DEPENDENCIES%\qtpropertybrowser-2.5_1\lib\QtSolutions_PropertyBrowser-2.5d.dll" bin\\Debug

rem copy xsd from code source
mkdir %GASS_HOME%\samples\GASSEd\bin\configuration\schema
cd "%GASS_HOME%\\source"
FOR /R %%X IN (*.xsd) DO copy "%%X" %GASS_HOME%\samples\GASSEd\bin\configuration\schema\

pause



