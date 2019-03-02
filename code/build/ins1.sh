#!/bin/sh
cd ..
g++ src/main.cpp `pkg-config --cflags --libs opencv`
cd build/
cmake ../
make
./seg large14.png config1.txt image5.png
