#!/bin/bash
mkdir build
javac -d ./build Chamber.java
cd ./build
echo "Main-Class: AdventOfCode.Chamber" > MANIFEST.MF
jar cvmf MANIFEST.MF ../Day17.jar AdventOfCode
