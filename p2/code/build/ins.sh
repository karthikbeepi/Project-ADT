#!/bin/sh
cd ..
g++ src/main.cpp `pkg-config --cflags --libs opencv`
cd build/
cmake ../
make
./sc castle.jpg 350 240 castle2.jpg
