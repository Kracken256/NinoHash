#!/bin/sh

# Compile crypto.c
gcc ninohash.c -o ninohash.o -O3 -c

# Compile main.c
gcc main.c -o nino256sum -O3 ninohash.o  -static

strip --strip-all nino256sum
rm ninohash.o
