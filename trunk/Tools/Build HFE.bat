@echo off

cls
Echo To make this compile-script work you have to make sure Dependencies is compiled with correct VS-version

:start
ECHO
Echo 2003
Echo 2005
Echo 2008
rem Echo Clean
set choice=
set /p choice=Enter what VS version do you intend to use: 
if not '%choice%'=='' set choice=%choice:~0,4%
if '%choice%'=='2003' goto VS2003
if '%choice%'=='2005' goto VS2005
if '%choice%'=='2008' goto VS2008
rem if '%choice%'=='Clea' goto CLEANUP

Echo "%choice%" is not valid please try again
GOTO START
:VS2003
call "%VS71COMNTOOLS%vsvars32.bat"
GOTO END
:VS2005
call "%VS80COMNTOOLS%vsvars32.bat"
GOTO END
:VS2008
call "%VS90COMNTOOLS%vsvars32.bat"
GOTO END
:END
SET VSVERSION=vs%choice%

cd Dependencies
SET HFE_BUILDLOG=%CD%\HFE-%date:~12,2%_%date:~4,2%_%date:~7,2%-%time:~0,2%_%time:~3,2%_%time:~6,2%.log


Rem There is a .sln-file for vs2003 and vs2005, if target is 2008 upgrade 2005-version 
if '%VSVERSION%'=='vs2003' ( 
	SET OGRESLNFILE=Ogre.sln
	SET PROJSUFFIX=
)
if '%VSVERSION%'=='vs2008' devenv %OGRESLNFILE% /upgrade


FOR %%B IN ("Debug|Win32" "Release|Win32") DO (
	Rem devenv %OGRESLNFILE% /clean %%B
	Rem To prevent building of all projects in Ogre soulution file, list them here
	FOR %%A IN (OgreMain Plugin_BSPSceneManager Plugin_CgProgramManager Plugin_OctreeSceneManager Plugin_ParticleFX ReferenceAppLayer OgreGUIRenderer RenderSystem_GL) DO (
		if exist C:\Program Files\Xoreax\IncrediBuild\BuildConsole.exe
		"C:\Program Files\Xoreax\IncrediBuild\BuildConsole.exe" %OGRESLNFILE% /prj=%%A%PROJSUFFIX% /build /cfg=%%B >> %HFE_DEP_BUILDLOG%
	)

	"C:\Program Files\Xoreax\IncrediBuild\BuildConsole.exe" %OGRESLNFILE% /prj="RenderSystem_Direct3D9"%PROJSUFFIX% /build /cfg=%%B /USEENV >> %HFE_DEP_BUILDLOG%
)