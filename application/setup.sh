#!/bin/bash

# Initialize and update submodules
git submodule update --init --recursive

# Build CMake project
cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -B build 

# Announce successful setup
echo "Build completed. You can now run your application."