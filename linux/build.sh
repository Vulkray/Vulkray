#!/bin/bash
# Runs ninja on debug/release build configuration
# NOTE: Execute file from project root directory!

# (Script used with IDE script configurations)
# (Set script options to either "debug" or "release")

DEBUG_DIR="build-debug"
RELEASE_DIR="build-release"

if [ "$1" = "debug" ]
then
  cd ./$DEBUG_DIR || exit
elif [ "$1" = "release" ]
then
  cd ./$RELEASE_DIR || exit
else
  echo "Invalid script argument! (Valid arguments: 'debug', 'release')"
fi

# Run build with Ninja
ninja
./bin/VulkrayExampleProgram