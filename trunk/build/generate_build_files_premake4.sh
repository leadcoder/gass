#!/bin/sh
chmod +x ../tools/premake4
../tools/premake4 --file=GASSSim4sln.lua --cc=gcc --os=linux gmake
../tools/premake4 --file=CoreTest4sln.lua --cc=gcc --os=linux gmake

#../tools/premake4 --file=GASSSim4sln.lua --cc=gcc --os=linux codeblocks
#../tools/premake4 --file=CoreTest4sln.lua --cc=gcc --os=linux codeblocks

