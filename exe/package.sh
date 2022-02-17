#!/bin/sh
set -e -x

cd ../build
make clean
rm -rf fsml.exe
make -j8

cp ../README.md README.txt
cp release/fsml.exe .
/c/Program\ Files/7-Zip/7z.exe a fsml.zip fsml.exe README.txt
cp fsml.zip ..
rm README.txt fsml.exe
