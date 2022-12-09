import sys
import numpy as np

if len(sys.argv)!=2:
  print("Run program with command line argument for input file name")
  sys.exit(255)

def motion(length):
 # positions
 x=[0 for _ in range(length)]
 y=[0 for _ in range(length)]

 # all tail positions
 visits={(0,0)}

 with open(sys.argv[1],'r') as f:
  for line in f:
    for _ in range(int(line[2:])):
      if line[0]=='R':
          x[0]=x[0]+1
      elif line[0]=='L':
          x[0]=x[0]-1
      elif line[0]=='U':
          y[0]=y[0]+1
      elif line[0]=='D':
          y[0]=y[0]-1
      else:
          sys.exit(255)
      
      for i in range(1,length):
        if abs(x[i-1]-x[i])>=2 or abs(y[i-1]-y[i])>=2:
          x[i]=x[i]+np.sign(x[i-1]-x[i])
          y[i]=y[i]+np.sign(y[i-1]-y[i])
      visits.add((x[length-1],y[length-1]))

 return len(visits)
 
 
print("Number of visited positions for tail of rope of length 2:")
print(motion(2))
print("Number of visited positions for tail of rope of length 10:")
print(motion(10))

