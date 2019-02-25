#!/bin/sh
cd ..
g++ src/main1.cpp `pkg-config --cflags --libs opencv`
cd build/
cmake ../
make
./seg simple.png config.txt image2.png
