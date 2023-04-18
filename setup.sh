#!/bin/sh

BUILD_TYPES=("Debug" "Release")

rm -r build &> /dev/null
mkdir build
cd build

for build_type in "${BUILD_TYPES[@]}"; do
    mkdir "$build_type"
    cd "$build_type"
    cmake -DCMAKE_BUILD_TYPE="$build_type" ../../
    make
    cd ..
done
