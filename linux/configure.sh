#!/bin/bash
# Creates and configures the debug/release build directories
# NOTE: Execute file from project root directory!

# (Script used with IDE script configurations)
# (Set script options to either "debug" or "release")

DEBUG_BUILD_DIR="build-debug"
RELEASE_BUILD_DIR="build-release"

if [ "$1" = "debug" ]
then
  mkdir $DEBUG_BUILD_DIR
  cd $DEBUG_BUILD_DIR || exit
  conan install .. --build=missing
  cmake .. -G "Ninja" -D CMAKE_BUILD_TYPE="Debug"

elif [ "$1" = "release" ]
then
  mkdir $RELEASE_BUILD_DIR
  cd $RELEASE_BUILD_DIR || exit
  conan install .. --build=missing
  cmake .. -G "Ninja" -D CMAKE_BUILD_TYPE="Release"
else
  echo "Invalid script argument! (Valid arguments: 'debug', 'release')"
fi
