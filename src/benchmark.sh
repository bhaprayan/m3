#!/bin/bash

make clean
make mapgen
./mapgen -n 32000 -m 3200 -t 1 -I
./mapgen -n 32000 -m 3200 -t 2 -I
./mapgen -n 32000 -m 3200 -t 4 -I
./mapgen -n 32000 -m 3200 -t 8 -I
./mapgen -n 32000 -m 3200 -t 12 -I
