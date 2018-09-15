#!/bin/sh
set -e

NDK_PATH=$(printenv ANDROID_NDK_HOME)
TOOL_CHAIN=$NDK_PATH/build/cmake/android.toolchain.cmake
API=14

ARCH[0]="armeabi"
ARCH[1]="armeabi-v6"
ARCH[2]="armeabi-v7a"
ARCH[3]="arm64-v8a"
ARCH[4]="mips"
ARCH[5]="mips64"
ARCH[6]="x86"
ARCH[7]="x86_64"

for i in "${ARCH[@]}"
do
        BUILD_DIR=build/droid/$i

        mkdir -p $BUILD_DIR/Debug

        cmake   -DCMAKE_SYSTEM_NAME=Android\
                -DCMAKE_TOOLCHAIN_FILE=$TOOL_CHAIN\
                -DCMAKE_SYSTEM_VERSION=$API\
                -DCMAKE_ANDROID_NDK=$NDK_PATH\
                -DCMAKE_ANDROID_ARCH_ABI=$i\
                -B$BUILD_DIR/Debug -H.\
                -DCMAKE_BUILD_TYPE=Debug\
                -DCMAKE_INSTALL_PREFIX=../build/droid/$i/debug

        make install -C $BUILD_DIR/Debug

        mkdir -p $BUILD_DIR/Release

        cmake   -DCMAKE_SYSTEM_NAME=Android\
                -DCMAKE_TOOLCHAIN_FILE=$TOOL_CHAIN\
                -DCMAKE_SYSTEM_VERSION=$API\
                -DCMAKE_ANDROID_NDK=$NDK_PATH\
                -DCMAKE_ANDROID_ARCH_ABI=$i\
                -B$BUILD_DIR/Release -H.\
                -DCMAKE_BUILD_TYPE=Release\
                -DCMAKE_INSTALL_PREFIX=../build/droid/$i/release

        make install -C $BUILD_DIR/Release
done
