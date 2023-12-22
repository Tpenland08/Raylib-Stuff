#!/bin/bash
#
#

brew install raylib
eval cc main.c -I/opt/homebrew/Cellar/raylib/5.0/include -L/opt/homebrew/Cellar/raylib/5.0/lib -lraylib -o main
./main