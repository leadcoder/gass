rem setup msvc
call "%VS110COMNTOOLS%vsvars32.bat"


call clean.bat

call checkout.bat

mkdir build

rem call build_platform.bat MSVC2010_x86 "Visual Studio 10"
call build_platform.bat MSVC2012_x86 "Visual Studio 11"
call build_platform.bat MSVC2012_x64 "Visual Studio 11 Win64"

call deploy.bat

pause
