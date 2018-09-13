#!/bin/sh
set -e

BUILD_DIR=build/web
EMSCRIPTEN=$(printenv EMSCRIPTEN)
TOOLCHAIN=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake

mkdir -p $BUILD_DIR

mkdir -p $BUILD_DIR/Debug
cmake  -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN -B$BUILD_DIR/Debug -H. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../build/web/debug
make install -C $BUILD_DIR/Debug

mkdir -p $BUILD_DIR/Release
cmake  -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN -B$BUILD_DIR/Release -H. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../build/web/release
make install -C $BUILD_DIR/Release
