#!/bin/bash

make clean
make mapgen
./mapgen > test.txt
python3 src/visualize.py
