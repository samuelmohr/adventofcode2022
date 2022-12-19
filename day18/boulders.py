import sys
import re

from operator import itemgetter


if len(sys.argv)!=2:
  print("Run program with command line argument for input file name")
  sys.exit(255)

cubes=[]

# read

prog = re.compile("^(-?\d+),(-?\d+),(-?\d+)$")
with open(sys.argv[1],'r') as f:
  for line in f:
    result = prog.match(line)
    if result==None:
          sys.exit(255)
    
    cubes.append((int(result.group(1)),int(result.group(2)),int(result.group(3))))

# PART 1
def surface(cubes):
 sides=0
 for cube in cubes:
  if (cube[0]-1,cube[1],cube[2]) not in cubes:
    sides+=1
  if (cube[0]+1,cube[1],cube[2]) not in cubes:
    sides+=1
  if (cube[0],cube[1]-1,cube[2]) not in cubes:
    sides+=1
  if (cube[0],cube[1]+1,cube[2]) not in cubes:
    sides+=1
  if (cube[0],cube[1],cube[2]-1) not in cubes:
    sides+=1
  if (cube[0],cube[1],cube[2]+1) not in cubes:
    sides+=1
  
 return sides

sides=surface(cubes)
print("Surface area:")
print(sides)

# PART 2
print("\nPART 2")

m1=min(cubes,key=itemgetter(0))[0]
M1=max(cubes,key=itemgetter(0))[0]
m2=min(cubes,key=itemgetter(1))[1]
M2=max(cubes,key=itemgetter(1))[1]
m3=min(cubes,key=itemgetter(2))[2]
M3=max(cubes,key=itemgetter(2))[2]

empty=[(a,b,c) for a in range(m1,M1+1) for b in range(m2,M2+1) for c in range(m3,M3+1) if (a,b,c) not in cubes]
tocare=set([cube for cube in empty if cube[0]==m1 or cube[0]==M1 or cube[1]==m2 or cube[1]==M2 or cube[2]==m3 or cube[2]==M3])

# delete
while len(tocare)!=0:
  cube=tocare.pop()
  empty.remove(cube)
  if (cube[0]-1,cube[1],cube[2]) in empty:
    tocare.add((cube[0]-1,cube[1],cube[2]))
  if (cube[0]+1,cube[1],cube[2]) in empty:
    tocare.add((cube[0]+1,cube[1],cube[2]))
  if (cube[0],cube[1]-1,cube[2]) in empty:
    tocare.add((cube[0],cube[1]-1,cube[2]))
  if (cube[0],cube[1]+1,cube[2]) in empty:
    tocare.add((cube[0],cube[1]+1,cube[2]))
  if (cube[0],cube[1],cube[2]-1) in empty:
    tocare.add((cube[0],cube[1],cube[2]-1))
  if (cube[0],cube[1],cube[2]+1) in empty:
    tocare.add((cube[0],cube[1],cube[2]+1))

print("Outer surface area:")
print(sides-surface(empty))

