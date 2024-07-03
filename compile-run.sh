#!/bin/bash

gcc -Wall -O2 -o infinity-matrix-v1 infinity-matrix-v1.c hash.c

time ./infinity-matrix-v1 < tabla.in
