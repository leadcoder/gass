

call setup_dep %1


echo BOOST_HOME = %BOOST_HOME%
echo GASS_DEPENDENCIES = %GASS_DEPENDENCIES%

cd build
mkdir %1
cd  %1

cmake.exe -G"%~2" ../../repo/trunk -DCMAKE_INSTALL_PREFIX=%CD%/../../install/%1

devenv.exe GASS.sln /build  "Debug" /project INSTALL  /Out "debug_build_log.txt"
devenv.exe GASS.sln /build  "Release" /project INSTALL  /Out "release_build_log.txt"

cd ../../


