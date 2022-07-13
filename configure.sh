#!/bin/bash
# Creates and configures build directory
mkdir build
cd build || exit
conan install .. --build=missing
cmake .. -G Ninja -D CMAKE_BUILD_TYPE=Debug
