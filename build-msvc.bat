set VCPKG_HOME=d:\vcpkg

rem cmake --preset "Windows Release"

set GASS_GENERATOR="Visual Studio 16 2019"
set GASS_TOOLCHAIN=%VCPKG_HOME%\scripts\buildsystems\vcpkg.cmake
set GASS_TRIPLET=x64-windows
set GASS_BUILD_FOLDER=%~dp0/build_msvc_2019
set GASS_SOURCE=%~dp0

rem rmdir /S /Q %BUILD_FOLDER%

@echo "Start cmake configure"
cmake %GASS_SOURCE% -B %GASS_BUILD_FOLDER%  -A x64 -G %GASS_GENERATOR% -DVCPKG_TARGET_TRIPLET=%GASS_TRIPLET% -DCMAKE_TOOLCHAIN_FILE=%GASS_TOOLCHAIN%

@echo "Start cmake build"
cmake --build %GASS_BUILD_FOLDER% --target INSTALL --config Debug
cmake --build %GASS_BUILD_FOLDER% --target INSTALL --config Release

pause