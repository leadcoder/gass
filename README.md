# GASS
C++ Game Engine


## Build instructions windows, MSVC 2022
### Prerequisites
  - Visual Studio 2022
  - CMake
  
### Build Dependencies (vcpkg)

Setup vcpkg by
1. Clone vcpkg from https://github.com/microsoft/vcpkg 
2. Checkout last verified commit
3. Bootstrap

```
> git clone https://github.com/microsoft/vcpkg
> cd vcpkg
> git checkout 5a2324f6667233aeb903d3117f6fd259a2be6f8b
> bootstrap-vcpkg.bat
```

### Build GASS CMake

Generate solution
```
> mkdir build
> cd build
> cmake -G "Visual Studio 17 2022" <path-to-cloned-gass-repo> -DCMAKE_TOOLCHAIN_FILE=<path-to-cloned-vcpkg-repo>/scripts/buildsystems/vcpkg.cmake
```

Build release from command line
```
> cmake.exe --build . --target ALL_BUILD --config Release
```
