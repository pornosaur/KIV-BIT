#!/bin/bash

dir="$(pwd)/build"

if test -d $dir; then
	rm -r build
fi

mkdir build
cd build
cmake ../.
make
cd ..

echo "BUILD SUCCESSUL"
