#!/bin/sh
cd ..
g++ src/main.cpp `pkg-config --cflags --libs opencv`
cd build/
cmake ../
make
./sc rsz_castle.jpg 200 240 testo.jpg
