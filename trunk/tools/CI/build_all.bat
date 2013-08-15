
call clean.bat

call svn_checkout.bat
rem call svn_update.bat

mkdir build

call "%VS100COMNTOOLS%vsvars32.bat"
call build_platform.bat MSVC2010_x86 "Visual Studio 10"
call build_platform.bat MSVC2010_x64 "Visual Studio 10 Win64"

call "%VS110COMNTOOLS%vsvars32.bat"
call build_platform.bat MSVC2012_x86 "Visual Studio 11"
call build_platform.bat MSVC2012_x64 "Visual Studio 11 Win64"

call deploy.bat

pause
