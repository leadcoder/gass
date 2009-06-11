@echo off
cls


:start
ECHO
echo 1) Open Scene Graph
echo 2) Ogre

set choice=
set /p choice=Enter what graphic system you intend to use: 
if not '%choice%'=='' set choice=%choice:~0,4%
if '%choice%'=='1' goto OSG
if '%choice%'=='2' goto OGRE

Echo "%choice%" is not valid please try again
GOTO START
:OGRE
copy configurations\ogre\*.xml .
GOTO END
:OSG
copy configurations\osg\*.xml .
GOTO END

:END
pause