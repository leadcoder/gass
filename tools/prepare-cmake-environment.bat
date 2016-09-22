@ECHO OFF 
goto :main
rem Function that checks if path is valid and set variable sent as first paramter to path value sent as second parameter
rem Usage: CALL :SetEnv <variable name> "<path that variable name should point to>"
rem		i.e: CALL :SetEnv BOOST_ROOT "%CD%\boost_1_57_0"
rem Parameter1 - variable name
rem Parameter2 - path that variable name in paramter 1 will be pointing to
:SetEnv
SETLOCAL
SETX %~1 %~2 >NUL
IF NOT EXIST %~2\NUL echo Invalid path: %~2
echo Set %~1 to	%~2
ENDLOCAL
goto :eof
:main

rem GASS Core etc
CALL :SetEnv BOOST_ROOT "%CD%/boost_1_57_0"
CALL :SetEnv BOOST_LIBRARYDIR "%BOOST_ROOT%/lib64-msvc-10.0"
CALL :SetEnv TBB_INSTALL_DIR "%CD%/tbb/"
CALL :SetEnv TBB_ARCH_PLATFORM "intel64/vc10"
CALL :SetEnv TINYXML2_HOME  "%CD%/tinyxml2"

rem GASS sim
CALL :SetEnv ANGELSCRIPT_HOME  "%CD%/angelscript"

rem GASS plugins

CALL :SetEnv OGRE_HOME "%CD%/Ogre-v1-9"
CALL :SetEnv OSGDIR "%CD%/OpenSceneGraph-v3.2.1"
CALL :SetEnv PHYSX_HOME "%CD%/PhysX-v3.3.2"
CALL :SetEnv OPENALDIR "%CD%/OpenAL1.1"
CALL :SetEnv MYGUI_HOME "%CD%/MyGUI_3.2.0"
CALL :SetEnv HYDRAX_HOME  "%CD%/Hydrax-v0.5.1"
CALL :SetEnv SKYX_HOME  "%CD%/SkyX-v0.4"
CALL :SetEnv RAKNET_HOME  "%CD%/RakNet"
CALL :SetEnv OIS_HOME  "%CD%/ois"
CALL :SetEnv ODE_HOME  "%CD%/ode"
CALL :SetEnv PAGEDGEOMETRY_HOME  "%CD%/PagedGeometry-1.1.1"

rem GASS QT apps, GASSEd
CALL :SetEnv QTPROPERTYBROWSER_HOME "%CD%/qtpropertybrowser-2.5_1"

pause

