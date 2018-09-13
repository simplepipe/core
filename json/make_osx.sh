#!/bin/sh
set -e

BUILD_DIR=build/osx

mkdir -p $BUILD_DIR

mkdir -p $BUILD_DIR/Debug
cmake -B$BUILD_DIR/Debug -H. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../build/osx/debug
make install -C $BUILD_DIR/Debug

mkdir -p $BUILD_DIR/Release
cmake -B$BUILD_DIR/Release -H. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../build/osx/release
make install -C $BUILD_DIR/Release
