import sys
import numpy as np

import re

import scanline

rowOfInterest=2000000

if len(sys.argv)!=2:
  print("Run program with command line argument for input file name")
  sys.exit(255)

beacons=[]
sensors=[]

# read

prog = re.compile("^Sensor at x=(-?\d+), y=(-?\d+): closest beacon is at x=(-?\d+), y=(-?\d+)$")
with open(sys.argv[1],'r') as f:
  for line in f:
    result = prog.match(line)
    if result==None:
          sys.exit(255)
    
    beacons.append((int(result.group(3)),int(result.group(4))))
    sensors.append((int(result.group(1)),int(result.group(2))))



# PART 1
rowOfInterest=int(input("Which row is of interest? "))

row=scanline.getLineFreeOfBeacons(sensors,beacons,rowOfInterest)

print("Number of positions that cannot contain a beacon:")
print(scanline.lenOfFOB(row))

# part 2
print("\nPART 2")
range_min=int(input("Minimum coordinate of interest: "))
range_max=int(input("Maximum coordinate of interest: "))
found_beacons=[]
for col in range(range_min,range_max+1):
  for l in scanline.spotsInFOB(scanline.getLineFreeOfBeacons(sensors,beacons,col),range_min,range_max):
    found_beacons.append((l,col))

print("Positions for Beacons:")
print(found_beacons)

print("Tuning frequence of unknown:")
for b in set(found_beacons)-set(beacons):
  print(b[0]*4000000+b[1])
