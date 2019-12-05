#!/bin/sh

SIZE=$1

make -j
time ./a4 -input ../data/horse_100k.txt -output horse.png -size $SIZE $SIZE