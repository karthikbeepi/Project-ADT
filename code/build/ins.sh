#!/bin/sh
cd ..
g++ src/main.cpp `pkg-config --cflags --libs opencv`
cd build/
cmake ../
make
./seg simple.png config.txt image3.png
