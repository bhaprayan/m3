#!/bin/bash

make
./mapgen > test.txt
python3 visualize.py
