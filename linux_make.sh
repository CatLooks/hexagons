#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage:"
    echo
    echo "make delete   - Deletes the build environment."
    echo "make generate - Generates a build environment."
    echo "make build    - Builds the project."
    echo "make run      - Runs the executable."
    exit 0
fi

if [ "$1" = "delete" ]; then
    if [ ! -d "build" ]; then
        echo "No build environment detected."
        exit 0
    fi
    echo "Deleting the build environment..."
    rm -rf build
    echo "Done."
    exit 0
fi

if [ "$1" = "generate" ]; then
    if [ ! -d "build" ]; then
        mkdir build
    fi
    echo "Generating a build environment..."
    cmake -S . -B build
    echo "Done."
    exit 0
fi

if [ "$1" = "build" ]; then
    if [ ! -d "build" ]; then
        echo "Build environment is not generated."
        exit 0
    fi
    echo "Building the project..."
    cmake --build build
    echo "Done."
    exit 0
fi

if [ "$1" = "run" ]; then
    if [ ! -d "build" ]; then
        echo "Build environment is not generated."
        exit 0
    fi
    if [ ! -f "build/bin/main" ]; then
        echo "Build 'Debug' does not exist."
        exit 0
    fi
    ./build/bin/main
    exit 0
fi
