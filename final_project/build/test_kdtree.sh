#!/bin/sh

SIZE=$1

make -j
time ./a4 -input ../data/bunny_4k.txt -output bunny.png -size $SIZE $SIZE
time ./a4 -input ../data/dragon_50k.txt -output dragon.png -size $SIZE $SIZE
time ./a4 -input ../data/horse_100k.txt -output horse.png -size $SIZE $SIZE