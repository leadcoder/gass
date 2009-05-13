@echo off

cls

echo 1) Visual Studio 7.1
echo 2) Visual Studio 8.0
echo 3) Visual Studio 9.0

:start
set choice=
set /p choice=What kind of build files do you want to generate?
if not '%choice%'=='' set choice=%choice:~0,2%
if '%choice%'=='1' goto 1
if '%choice%'=='2' goto 2
if '%choice%'=='3' goto 3
ECHO "%choice%" is not valid please try again
ECHO.
goto start


:1
cd ..\HiFiEngine\Scripts
..\..\HFETools\premake.exe --target vs2003 --os windows
cd ..\..\HiFiAppFramework\Scripts
..\..\HFETools\premake.exe --target vs2003 --os windows
cd ..\..\HFEDemo\Scripts
..\..\HFETools\premake.exe --target vs2003 --os windows
goto eof

:2
cd ..\HiFiEngine\Scripts
..\..\HFETools\premake.exe --target vs2005 --os windows
cd ..\..\HiFiAppFramework\Scripts
..\..\HFETools\premake.exe --target vs2005 --os windows
cd ..\..\HFEDemo\Scripts
..\..\HFETools\premake.exe --target vs2005 --os windows
goto eof

:3
cd ..\HiFiEngine\Scripts
..\..\HFETools\premake.exe --target vs2008 --os windows
cd ..\..\HiFiAppFramework\Scripts
..\..\HFETools\premake.exe --target vs2008 --os windows
cd ..\..\HFEDemo\Scripts
..\..\HFETools\premake.exe --target vs2008 --os windows

:eof
pause