@echo off

cls

echo 1) Visual Studio 8.0 (2005)
echo 2) Visual Studio 9.0 (2008)
echo 3) Visual Studio 10.0 (2010)

:start
set choice=
set /p choice="What kind of build files do you want to generate? "
if not '%choice%'=='' set choice=%choice:~0,2%
if '%choice%'=='1' (
set target=vs2005
goto GENERATE
)
if '%choice%'=='2' (
set target=vs2008
goto GENERATE
)
if '%choice%'=='3' (
set target=vs2010
goto GENERATE
)
ECHO "%choice%" is not valid please try again
ECHO.
goto start

:GENERATE

..\Tools\premake4.exe --file=GASSSim4sln.lua  --os=windows %target%
..\Tools\premake4.exe --file=CoreTest4sln.lua  --os=windows %target%


rem cls
pause
goto endoffile

set choice=
set /p choice="Do you want to build GASS? (Y/N)"
if '%choice%'=='y' (
goto BUILDGASS
)
if '%choice%'=='Y' (
goto BUILDGASS
)

goto ENDOFFILE


:BUILDGASS


Echo To proceed environment variables OGRE_HOME, OGRE_PATH need to be defined. 
Echo When you have made sure that they are

pause


call "Build GASS.bat" %target%


:ENDOFFILE

@echo on