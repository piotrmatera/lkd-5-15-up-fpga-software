#!/bin/bash

python=/c/Users/Piotr/AppData/Local/Programs/Python/Python36/python.exe

extractor=$1 
hex_file1=$2 

#extractor=/c/_Projekty/Bartek/projects/kompensator/src/hex-id-py/src/test.py 
#hex_file1=/c/_Projekty/Bartek/projects/kompensator/src/Kompensator/cpu01/DebugTI/cpu01.hex

$python $extractor $hex_file1
