#!/bin/bash
echo "start the project"
gcc -o test analyinfile.c ../src/* -I ../include/ -I ../nick-include/ -lm -L /usr/local/lib -lzmq
