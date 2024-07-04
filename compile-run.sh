#!/bin/bash

gcc -Wall -O2 -o infinity-war-v2 infinity-war-v2.c hash.c

time ./infinity-war-v2 < tabla.in
