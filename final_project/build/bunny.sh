#!/bin/sh

SIZE=$1

make -j
time ./a4 -input ../data/bunny_4k.txt -output bunny.png -size $SIZE $SIZE