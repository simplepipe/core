#!/bin/sh
set -e

BUILD[0]=zip
BUILD[1]=png
BUILD[2]=jpeg
BUILD[3]=math
BUILD[4]=kernel
BUILD[5]=xml
BUILD[6]=json
BUILD[7]=collada
BUILD[8]=action
BUILD[9]=opengl
BUILD[10]=gui
BUILD[11]=physics

for i in "${BUILD[@]}"
do
        case $1 in
                droid|ios|osx|linux|web)
                        rm -rf build/$1
                        cd $i
                        rm -rf build/$1
                        cd ..
                        ;;
                all)
                        rm -rf build
                        cd $i
                        rm -rf build
                        cd ..
                        ;;
        esac
done
