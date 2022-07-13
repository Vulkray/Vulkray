#!/bin/bash
# Creates and configures build directory

# (Script used with IDE script configurations)
# (Set script options to either "Debug" or "Release")

mkdir build
cd build || exit
conan install .. --build=missing
cmake .. -G Ninja -D CMAKE_BUILD_TYPE="$1"
