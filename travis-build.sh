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

export GASS_DEP_ROOT=`pwd`/gass-dep-install
export SOURCE_ROOT=`pwd`/master
mkdir build
cd build

#tinyxml2
mkdir tinyxml2
cd tinyxml2
cmake -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$GASS_DEP_ROOT/tinyxml2/ $SOURCE_ROOT/tinyxml2/
make install
cd ..

cd ..
cd ..

export TINYXML2_HOME=$GASS_DEP_ROOT/tinyxml2
#printenv TINYXML2_HOME
#export PAGEDGEOMETRY_HOME=$GASS_DEP_ROOT/PagedGeometry-1.1.1
#export ODE_HOME=$GASS_DEP_ROOT/ODE
#export HYDRAX_HOME=$GASS_DEP_ROOT/Hydrax-v0.5.1
#export SKYX_HOME=$GASS_DEP_ROOT/SkyX-v0.4
#export ANGELSCRIPT_HOME=$GASS_DEP_ROOT/angelscript
#export RAKNET_HOME=$GASS_DEP_ROOT/RakNet
#export INSTALL_ROOT=`pwd`/gass-install
export SOURCE_ROOT=`pwd`/

mkdir build
cd build

cmake -DCMAKE_EXE_LINKER_FLAGS="-Wl,--no-as-needed" -DCMAKE_BUILD_TYPE=Release  -DGASS_BUILD_CORE_SAMPLES=ON -DGASS_BUILD_SIM_SAMPLES=OFF -DGASS_BUILD_SIM=OFF  $SOURCE_ROOT

make

