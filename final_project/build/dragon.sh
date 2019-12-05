#!/bin/sh

SIZE=$1

make -j
time ./a4 -input ../data/dragon_50k.txt -output dragon.png -size $SIZE $SIZE