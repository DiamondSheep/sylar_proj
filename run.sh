#!/bin/sh

if ! [ -d ./build ]
then
    mkdir build
fi

if ! [ -d ./data ]
then
    mkdir data
    touch data/log.txt
fi

cd build
cmake .. 
make -j${nproc}
cd ..

cd bin
./test