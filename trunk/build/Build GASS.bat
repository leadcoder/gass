@echo off
SETLOCAL ENABLEEXTENSIONS

set VSVERSION=%1

if '%VSVERSION%'=='vs2003' goto VS2003
if '%VSVERSION%'=='vs2005' goto VS2005
if '%VSVERSION%'=='vs2008' goto VS2008
if '%VSVERSION%'=='vs2010' goto VS2010


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
:VS2010
call "%VS100COMNTOOLS%vsvars32.bat"
GOTO END
:END


set timmar=%time:~0,2%
if %timmar% LSS 10 echo (
	set timmar=%time:~1,1%
)
SET GASS_BUILDLOG=%CD%\GASS-%date%-%timmar%_%time:~3,2%_%time:~6,2%.log

goto BUILDGASS

:COMPILE
Rem Parameters: <solutionname> <projectname> <configurationname> (</USEENV>)
if exist "C:\Program Files\Xoreax\IncrediBuild\BuildConsole.exe". (
		echo "Building using C:\Program Files\Xoreax\IncrediBuild\BuildConsole.exe" 
		echo Solution: %1 Project: %2 Configuration: %3
	"C:\Program Files\Xoreax\IncrediBuild\BuildConsole.exe" %1 /prj=%2 /build /cfg=%3 %4 >> %GASS_BUILDLOG%
) else (
	if exist "C:\Program Files (x86)\Xoreax\IncrediBuild\BuildConsole.exe". (
		echo "Building using C:\Program Files (x86)\Xoreax\IncrediBuild\BuildConsole.exe"
		echo Solution: %1 Project: %2 Configuration: %3
		"C:\Program Files (x86)\Xoreax\IncrediBuild\BuildConsole.exe" %1 /prj=%2 /build /cfg=%3 %4 >> %GASS_BUILDLOG%
	) else (
		echo "Building using devenv.exe"
		echo Solution: %1 Project: %2 Configuration: %3
		devenv /build %3 %1 /project %2 %4 >> %GASS_BUILDLOG%
	)
)
goto :EOF

:COMPILESOLUTION
Rem Parameters: <solutionname> <projectname> <configurationname> (</USEENV>)
if exist "C:\Program Files\Xoreax\IncrediBuild\BuildConsole.exe". (
		echo "Building using C:\Program Files\Xoreax\IncrediBuild\BuildConsole.exe" 
	"C:\Program Files\Xoreax\IncrediBuild\BuildConsole.exe" %1 /build /cfg=%2 %3 >> %GASS_BUILDLOG%
) else (
	if exist "C:\Program Files (x86)\Xoreax\IncrediBuild\BuildConsole.exe". (
		echo "Building using C:\Program Files (x86)\Xoreax\IncrediBuild\BuildConsole.exe"
		"C:\Program Files (x86)\Xoreax\IncrediBuild\BuildConsole.exe" %1 /build /cfg=%2 %3 >> %GASS_BUILDLOG%
	) else (
		echo "Building using devenv.exe"
		devenv /build %2 %1 %3 >> %GASS_BUILDLOG%
	)
)
goto :EOF

:BUILDGASS

rem Build Debug
call :COMPILE GASSSim.sln "GASSCore" "Debug|Win32" /USEENV
call :COMPILE GASSSim.sln "GASSSim" "Debug|Win32" /USEENV
call :COMPILESOLUTION GASSSim.sln "Debug|Win32" /USEENV


REM call :COMPILE GASSSim.sln "GASSPluginOgre" "Debug|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginODE" "Debug|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginOIS" "Debug|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginOpenAL" "Debug|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginPagedGeometry" "Debug|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginGame" "Debug|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginBase" "Debug|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginRakNet" "Debug|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSEditorModule" "Debug|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginEnvironment" "Debug|Win32" /USEENV

rem Build Release
call :COMPILE GASSSim.sln "GASSCore" "Release|Win32" /USEENV
call :COMPILE GASSSim.sln "GASSSim" "Release|Win32" /USEENV
call :COMPILESOLUTION GASSSim.sln "Release|Win32" /USEENV


REM call :COMPILE GASSSim.sln "GASSPluginOgre" "Release|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginODE" "Release|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginOIS" "Release|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginOpenAL" "Release|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginPagedGeometry" "Release|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginGame" "Release|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginBase" "Release|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginRakNet" "Release|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSEditorModule" "Release|Win32" /USEENV
REM call :COMPILE GASSSim.sln "GASSPluginEnvironment" "Release|Win32" /USEENV

cd ..\..