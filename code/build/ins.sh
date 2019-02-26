#!/bin/sh
cd ..
g++ src/main1.cpp `pkg-config --cflags --libs opencv`
cd build/
cmake ../
make
./seg large.jpg config.txt image3.png
