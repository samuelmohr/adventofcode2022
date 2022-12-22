#!/bin/bash
mkdir build
javac -d ./build Numberlist.java
cd ./build
echo "Main-Class: AdventOfCode.Numberlist" > MANIFEST.MF
jar cvmf MANIFEST.MF ../Day20.jar AdventOfCode
