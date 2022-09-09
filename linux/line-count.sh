#!/bin/bash
# Prints the total lines of code in the engine source.
# NOTE: Execute file from project root directory!

CXX_FILES=$( ( find ./ -name '*.cxx' -print0 | xargs -0 cat ) | wc -l)
HEADER_FILES=$( ( find ./ -name '*.h' -print0 | xargs -0 cat ) | wc -l)
FRAG_SHADERS=$( ( find ./ -name '*.frag' -print0 | xargs -0 cat ) | wc -l)
VERT_SHADERS=$( ( find ./ -name '*.vert' -print0 | xargs -0 cat ) | wc -l)

TOTAL_LINES=$(( CXX_FILES + HEADER_FILES + FRAG_SHADERS + VERT_SHADERS ))
echo "Total Source Lines: $TOTAL_LINES"