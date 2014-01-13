set OUT_DIR=bin_x64

cd %OUT_DIR%
md Configuration
md Debug
cd Debug
md GASSPlugins
cd..
md Release
cd Release
md GASSPlugins
cd..
cd..

copy "GASS.xml" %OUT_DIR%\\configuration\\
copy "EditorApplication.xml" %OUT_DIR%\\configuration\\
copy "GASSControlSettings.xml" %OUT_DIR%\\configuration\\



copy "%GASS_HOME%\\bin\\debug\\GASSCore_d.dll" %OUT_DIR%\\debug
copy "%GASS_HOME%\\bin\\debug\\GASSSim_d.dll" %OUT_DIR%\\debug
copy "%GASS_HOME%\\bin\\debug\\GASSEditorModule_d.dll" %OUT_DIR%\\debug
copy "%GASS_HOME%\\bin\\debug\\GASSPluginOgre_d.dll" %OUT_DIR%\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginOSG_d.dll" %OUT_DIR%\\debug\\GASSPlugins

copy "%GASS_HOME%\\bin\\debug\\GASSPluginOIS_d.dll" %OUT_DIR%\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginODE_d.dll" %OUT_DIR%\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginPhysX3_d.dll" %OUT_DIR%\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginGame_d.dll" %OUT_DIR%\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginBase_d.dll" %OUT_DIR%\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginEnvironment_d.dll" %OUT_DIR%\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginPagedGeometry_d.dll" %OUT_DIR%\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginRaknet_d.dll" %OUT_DIR%\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginOpenAL_d.dll" %OUT_DIR%\\debug\\GASSPlugins
copy "%GASS_HOME%\\bin\\debug\\GASSPluginAI_d.dll" %OUT_DIR%\\debug\\GASSPlugins


copy "%GASS_HOME%\\bin\\release\\GASSCore.dll" %OUT_DIR%\\release
copy "%GASS_HOME%\\bin\\release\\GASSSim.dll" %OUT_DIR%\\release
copy "%GASS_HOME%\\bin\\release\\GASSEditorModule.dll" %OUT_DIR%\\release
copy "%GASS_HOME%\\bin\\release\\GASSPluginOgre.dll" %OUT_DIR%\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginOSG.dll" %OUT_DIR%\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginOIS.dll" %OUT_DIR%\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginODE.dll" %OUT_DIR%\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginPhysX3.dll" %OUT_DIR%\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginGame.dll" %OUT_DIR%\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginBase.dll" %OUT_DIR%\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginEnvironment.dll" %OUT_DIR%\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginPagedGeometry.dll" %OUT_DIR%\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginRaknet.dll" %OUT_DIR%\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginOpenAL.dll" %OUT_DIR%\\release\\GASSPlugins
copy "%GASS_HOME%\\bin\\release\\GASSPluginAI.dll" %OUT_DIR%\\release\\GASSPlugins

copy "%GASS_DEPENDENCIES%\lib\tbb_debug.dll" %OUT_DIR%\\Debug
copy "%GASS_DEPENDENCIES%\lib\tbb.dll" %OUT_DIR%\\Release

copy "%OGRE_HOME%\bin\debug\*_d.dll" %OUT_DIR%\\Debug\
copy "%OGRE_HOME%\bin\debug\cg.dll" %OUT_DIR%\\Debug\

copy "%OGRE_HOME%\bin\release\*.dll" %OUT_DIR%\\Release
copy "%OGRE_HOME%\bin\debug\cg.dll" %OUT_DIR%\\Release

copy "%GASS_DEPENDENCIES%\lib\ois_d.dll" %OUT_DIR%\\Debug
copy "%GASS_DEPENDENCIES%\lib\ois.dll" %OUT_DIR%\\Release

copy "%GASS_DEPENDENCIES%\lib\OpenAL32.dll" %OUT_DIR%\\Debug
copy "%GASS_DEPENDENCIES%\lib\OpenAL32.dll" %OUT_DIR%\\Release


copy "%QT_HOME%\bin\QtCore4.dll" %OUT_DIR%\\Release
copy "%QT_HOME%\bin\QtGui4.dll" %OUT_DIR%\\Release

copy "%QT_HOME%\bin\QtCored4.dll" %OUT_DIR%\\Debug
copy "%QT_HOME%\bin\QtGuid4.dll" %OUT_DIR%\\Debug

copy  "%GASS_DEPENDENCIES%\qtpropertybrowser-2.5_1\lib\QtSolutions_PropertyBrowser-2.5.dll" %OUT_DIR%\\Release
copy  "%GASS_DEPENDENCIES%\qtpropertybrowser-2.5_1\lib\QtSolutions_PropertyBrowser-2.5d.dll" %OUT_DIR%\\Debug

rem copy xsd from code source
rem mkdir %GASS_HOME%\samples\GASSEd\bin\configuration\schema
rem cd "%GASS_HOME%\\source"
rem FOR /R %%X IN (*.xsd) DO copy "%%X" %GASS_HOME%\samples\GASSEd\bin\configuration\schema\

pause



