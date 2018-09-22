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

for i in "${BUILD[@]}"
do
        cd $i
        bash make_$1.sh
        cd ..
done
