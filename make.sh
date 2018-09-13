#!/bin/sh
set -e

BUILD[0]=zip
BUILD[1]=png
BUILD[2]=jpeg
BUILD[3]=math
BUILD[4]=kernel
BUILD[5]=xml
BUILD[6]=action
BUILD[7]=opengl

for i in "${BUILD[@]}"
do
        cd $i
        bash make_$1.sh
        cd ..
done
