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
cmake -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_ROOT/tinyxml2/ $SOURCE_ROOT/tinyxml2/
make install
cd ..

cd ..
cd ..

