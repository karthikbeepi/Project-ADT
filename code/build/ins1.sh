#!/bin/sh
cd ..
g++ src/main1.cpp `pkg-config --cflags --libs opencv`
cd build/
cmake ../
make
./seg large8.png config1.txt image.png
