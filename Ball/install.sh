#!/bin/bash
#
#

brew install raylib
mkdir raylib-ball
cd raylib-ball
wget https://raw.githubusercontent.com/Tpenland08/Raylib-Stuff/main/Ball/main.c
eval cc main.c -I/opt/homebrew/Cellar/raylib/5.0/include -L/opt/homebrew/Cellar/raylib/5.0/lib -lraylib -o main
./main
