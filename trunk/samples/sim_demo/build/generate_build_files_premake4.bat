@echo off

cls

echo 1) Visual Studio 7.1
echo 2) Visual Studio 8.0
echo 3) Visual Studio 9.0

:start
set choice=
set /p choice="What kind of build files do you want to generate? "
if not '%choice%'=='' set choice=%choice:~0,2%
if '%choice%'=='1' (
set target=vs2003
goto GENERATE
)
if '%choice%'=='2' (
set target=vs2005
goto GENERATE
)
if '%choice%'=='3' (
set target=vs2008
goto GENERATE
)
ECHO "%choice%" is not valid please try again
ECHO.
goto start

:GENERATE
@echo on
..\..\..\Tools\premake4.exe --file=SimDemo4sln.lua  --os=windows %target%

pause