#!/bin/sh
cd ..
g++ src/main.cpp `pkg-config --cflags --libs opencv`
cd build/
cmake ../
make
./seg large10.png config1.txt image.png
