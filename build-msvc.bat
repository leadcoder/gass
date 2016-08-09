rem git clone https://github.com/leadcoder/gass master
rem cd master

rem Setup dependencies
set GASS_DEP_ROOT=F:/dev_zone/FOI-dep/MSVC2010_x64

rem GASS Core deps
SET BOOST_ROOT=%GASS_DEP_ROOT%/boost_1_57_0
SET BOOST_LIBRARYDIR=%BOOST_ROOT%/lib64-msvc-10.0
SET TBB_INSTALL_DIR=%GASS_DEP_ROOT%/tbb/
SET TBB_ARCH_PLATFORM=intel64/vc10
SET TINYXML2_HOME=%GASS_DEP_ROOT%/tinyxml2

rem GASS Sim deps
SET ANGELSCRIPT_HOME=%GASS_DEP_ROOT%/angelscript

rem GASS Plugins deps
SET OGRE_HOME=%GASS_DEP_ROOT%/Ogre-v1-9
SET OSGDIR=%GASS_DEP_ROOT%/OpenSceneGraph-v3.2.1
SET OSGEARTHDIR=%GASS_DEP_ROOT%/osgEarth

SET PHYSX_HOME=%GASS_DEP_ROOT%/PhysX-v3.3.2
SET OPENALDIR=%GASS_DEP_ROOT%/OpenAL1.1
SET MYGUI_HOME=%GASS_DEP_ROOT%/MyGUI_3.2.0
SET HYDRAX_HOME=%GASS_DEP_ROOT%/Hydrax-v0.5.1
SET SKYX_HOME=%GASS_DEP_ROOT%/SkyX-v0.4
SET RAKNET_HOME=%GASS_DEP_ROOT%/RakNet
SET OIS_HOME=%GASS_DEP_ROOT%/ois
SET ODE_HOME=%GASS_DEP_ROOT%/ode
SET PAGEDGEOMETRY_HOME=%GASS_DEP_ROOT%/PagedGeometry-1.1.1

rem GASS apps (GASSEd) deps 
rem SET QTPROPERTYBROWSER_HOME %GASS_DEP_ROOT%/qtpropertybrowser-2.5_1
rem Set Visual Studio version, make this optional?

set BUILD_NAME=MSVC2010_x64
set CMAKE_PROFILE="Visual Studio 10 Win64"

rem setup vs2010 devenv
call "%VS100COMNTOOLS%vsvars32.bat"

mkdir %BUILD_NAME%
cd %BUILD_NAME%

rem Generate solution
cmake.exe -G %CMAKE_PROFILE% ../ -DGASS_BUILD_SIM_SAMPLES=ON -DGASS_BUILD_PLUGIN_MYGUI=ON  -DGASS_BUILD_PLUGIN_OSGEARTH=ON -DCMAKE_INSTALL_PREFIX=%CD%/install > cmake_log_%BUILD_NAME%.txt

rem Build solution (debug and release)
devenv.exe GASS.sln /build  "Release" /project INSTALL  /Out "release_build_log_%BUILD_NAME%.txt"
devenv.exe GASS.sln /build  "Release" /project INSTALL  /Out "release_build_log_%BUILD_NAME%.txt"
devenv.exe GASS.sln /build  "Debug" /project INSTALL  /Out "debug_build_log_%BUILD_NAME%.txt"
devenv.exe GASS.sln /build  "Debug" /project INSTALL  /Out "debug_build_log_%BUILD_NAME%.txt"

pause