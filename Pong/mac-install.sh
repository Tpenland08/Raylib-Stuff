#!/bin/bash
#
#

brew install raylib
git clone "https://github.com/Tpenland08/Raylib-Stuff.git" Pong
cd Pong
rm -rf Ball
rm "README.md"
rm mac-install.sh
cp ./Pong/pong.c ./pong.c
cp -a ./Pong/assets/. ./assets
rm -rf ./Pong
eval cc pong.c -I/opt/homebrew/Cellar/raylib/5.0/include -L/opt/homebrew/Cellar/raylib/5.0/lib -lraylib -o Pong
rm pong.c