#!/bin/sh
set -e

BUILD[0]=zip
BUILD[1]=png
BUILD[2]=jpeg
BUILD[3]=haru
BUILD[4]=math
BUILD[5]=kernel
BUILD[6]=xml
BUILD[7]=json
BUILD[8]=collada
BUILD[9]=action
BUILD[10]=opengl
BUILD[11]=gui
BUILD[12]=physics

for i in "${BUILD[@]}"
do
        cd $i
        bash make_$1.sh
        cd ..
done
