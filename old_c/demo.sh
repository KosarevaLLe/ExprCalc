#!/bin/sh
gcc -g -Wall -o expr1 expr1.c bmp.c -lm
./expr1 << EOF
(x - 3) * (x - 3)/2 - 1
0
sin(x)
3.14
sin((x - 2) * (x - 2)) + 0.1
3.14
q
EOF
