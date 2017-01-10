#TBB
#sudo apt-get install libtbb-dev

#BOOST
#sudo-apt-get install boost-dev

#OGRE
#sudo apt-get install libogre-1.9-dev

#OSG
#sudo apt-get install libopenscenegraph-dev

mkdir gass_dep
cd gass_dep
git clone https://github.com/leadcoder/gass-dependencies master

export INSTALL_ROOT=`pwd`/gass-dep-install
export SOURCE_ROOT=`pwd`/master
mkdir build
cd build

#tinyxml2
mkdir tinyxml2
cd tinyxml2
cmake -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/tinyxml2/ $SOURCE_ROOT/tinyxml2/
make install
cmake -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/tinyxml2/ $SOURCE_ROOT/tinyxml2/
make install
cd ..

#ODE
mkdir ODE 
cd ODE
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/ODE/ $SOURCE_ROOT/ODE/
make install
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-fPIC  -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/ODE/ $SOURCE_ROOT/ODE/
make install
cd ..

#angelscript
mkdir angelscript 
cd angelscript
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/angelscript/ $SOURCE_ROOT/angelscript_2.28.0/angelscript/projects/cmake
make install
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-fPIC  -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/angelscript/ $SOURCE_ROOT/angelscript_2.28.0/angelscript/projects/cmake
make install
cd ..

#RakNet
mkdir RakNet 
cd RakNet
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/RakNet/ $SOURCE_ROOT/RakNet
make install
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-fPIC  -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/RakNet/ $SOURCE_ROOT/RakNet
make install
cd ..

#skyx
mkdir skyx
cd skyx
cmake -DBoost_USE_STATIC_LIBS=OFF -DSKYX_BUILD_SAMPLES=OFF -DCMAKE_MODULE_PATH=/usr/share/OGRE/cmake/modules -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/SkyX-v0.4/  $SOURCE_ROOT/SkyX-v0.4/
make install
cmake -DBoost_USE_STATIC_LIBS=OFF -DSKYX_BUILD_SAMPLES=OFF -DCMAKE_MODULE_PATH=/usr/share/OGRE/cmake/modules -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/SkyX-v0.4/  $SOURCE_ROOT/SkyX-v0.4/
make install
cd ..

#hydrax
mkdir Hydrax
cd Hydrax
cmake -DBoost_USE_STATIC_LIBS=OFF -DCMAKE_MODULE_PATH=/usr/share/OGRE/cmake/modules -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/Hydrax-v0.5.1/  $SOURCE_ROOT/Hydrax-v0.5.1/
make install
cmake -DBoost_USE_STATIC_LIBS=OFF -DCMAKE_MODULE_PATH=/usr/share/OGRE/cmake/modules -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/Hydrax-v0.5.1/  $SOURCE_ROOT/Hydrax-v0.5.1/
make install
cd ..

#PagedGeometry
mkdir PagedGeometry
cd PagedGeometry
cmake -DBoost_USE_STATIC_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/PagedGeometry-1.1.1/  $SOURCE_ROOT/PagedGeometry-1.1.1/
make install
cmake -DBoost_USE_STATIC_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/PagedGeometry-1.1.1/  $SOURCE_ROOT/PagedGeometry-1.1.1/
make install
cd ..

#RAKNET
mkdir RakNet
cd RakNet
cmake  -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/RakNet/  $SOURCE_ROOT/RakNet/
make install
cmake  -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/RakNet/  $SOURCE_ROOT/RakNet/
make install
cd ..
