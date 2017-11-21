# GASS
C++ Game Engine

Travis Build (master) [![Build status](https://travis-ci.org/leadcoder/gass.svg?branch=master)](https://travis-ci.org/leadcoder/gass)

## Build instructions MSVC
### Prerequisites
  - CMake
  - Git
  - (Optional) Doxygen
  
### Dependencies (prebuild)
  - Download dependencies from [here](https://www.dropbox.com/sh/ccvd9nfr3ef203g/AACfPrh5gLeGyMDkESLpG3E8a?dl=0)
  - Unzip to location of choice    
  - (Optional) Run prepare-cmake-environment.bat in the new dep folder, this will setup environment variables used by cmake to find dependencies

### Clone & Build
```
 git clone https://github.com/leadcoder/gass master
 cd master
```
You can now either:
- Edit build-msvc.bat and set GASS_DEP_ROOT to your dep folder
- Run build-msvc.bat

Or if you have done the optional step above (prepare-cmake-environment.bat) you can:
  - Start CMake GUI, configure GASS to your requirements and generate the solution.
  - Open solution in MSVC and build

