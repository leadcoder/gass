export VCPKG_ROOT=/path/to/vcpkg

export SOURCE_ROOT=`pwd`/
export VCPKG_OVERLAY_PORTS=`pwd`/tools/vcpkg/ports
export VCPKG_OVERLAY_TRIPLETS=`pwd`/tools/vcpkg/triplets
        
cmake -B=./build_release -DCMAKE_BUILD_TYPE=Release $SOURCE_ROOT -DGASS_BUILD_MODULE_OSGEARTH=ON -DGASS_BUILD_PLUGIN_OPENAL=OFF -DGASS_BUILD_PLUGIN_OIS=OFF -DVCPKG_TARGET_TRIPLET=x64-linux-dynamic -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build ./build_release



