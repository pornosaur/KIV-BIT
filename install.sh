#!/bin/bash

if [ -d "lib/" ]; then
	rm -r ./lib/	
fi

cd gmp-6.1.2
make distclean
cd ..

mkdir lib
cd lib
../gmp-6.1.2/configure --enable-cxx --prefix=$(pwd) --exec-prefix=$(pwd) --libdir=$(pwd)
make
make install
