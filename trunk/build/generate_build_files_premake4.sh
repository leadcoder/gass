#!/bin/sh
chmod +x ../tools/premake4

echo "1) Code::Blocks"
echo "2) GNU makefiles"
#echo "c) clean"

echo "What kind of build files do you want to generate?"
read answer
case $answer in
    1) export target=codeblocks;;
    2) export target=gmake;;
#    c)    echo You chose c;;
    *)    echo You did not chose 1 or 2 ;;
esac


../tools/premake4 --file=GASSSim4sln.lua --cc=gcc --os=linux $target
../tools/premake4 --file=CoreTest4sln.lua --cc=gcc --os=linux $target

