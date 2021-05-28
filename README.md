# GASS
C++ Game Engine

Travis Build (master) [![Build status](https://travis-ci.org/leadcoder/gass.svg?branch=master)](https://travis-ci.org/leadcoder/gass)

## Build instructions windows, MSVC 2019 
### Prerequisites
  - Visual Studio 2019
  - CMake
  
### Build Dependencies (vcpkg)

Setup vcpkg by
1. Clone vcpkg from https://github.com/microsoft/vcpkg 
2. Checkout last verified commit
3. Build vcpkg.exe
4. Set overlay ports folder, https://github.com/leadcoder/gass/tree/master/tools/vcpkg/ports.
```
> git clone https://github.com/microsoft/vcpkg
> cd vcpkg
> git checkout 546813ae7b9e2873dd3d38e78b27ac5582feae10
> bootstrap-vcpkg.bat
> set VCPKG_OVERLAY_PORTS=<path-to-cloned-gass-repo>/tools/vcpkg/ports
```
Install GASSSim deps:
```
> vcpkg install gass-deps:x64-windows
```
Note that you can install GASS using vcpkg if you depend on GASS in your own project
```
> vcpkg install gasssim:x64-windows
```
...or just GASSCore
```
> vcpkg install gasscore:x64-windows
```

### Build GASS CMake

Generate solution
```
> mkdir build
> cd build
> cmake -G"Visual Studio 16 2019" <path-to-cloned-gass-repo> -DCMAKE_TOOLCHAIN_FILE=<path-to-cloned-vcpkg-repo>/scripts/buildsystems/vcpkg.cmake
```

Build release from command line
```
> cmake.exe --build . --target ALL_BUILD --config Release
```
