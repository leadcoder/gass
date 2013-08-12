
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
copy "%GASS_HOME%\\bin\\debug\\GASSPluginOgre_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginOSG_d.dll" bin\\debug\\GASSPlugins

copy "%GASS_HOME%\\bin\\debug\\GASSPluginOIS_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginODE_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginGame_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginBase_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginEnvironment_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginPagedGeometry_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginRaknet_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginOpenAL_d.dll" bin\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginAI_d.dll" bin\\debug\\GASSPlugins


copy "%GASS_HOME%\\bin\\release\\GASSCore.dll" bin\\release
copy "%GASS_HOME%\\bin\\release\\GASSSim.dll" bin\\release
copy "%GASS_HOME%\\bin\\release\\GASSEditorModule.dll" bin\\release
copy "%GASS_HOME%\\bin\\release\\GASSPluginOgre.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginOSG.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginOIS.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginODE.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginGame.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginBase.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginEnvironment.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginPagedGeometry.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginRaknet.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginOpenAL.dll" bin\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginAI.dll" bin\\release\\GASSPlugins

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



