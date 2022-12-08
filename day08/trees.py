import sys


if len(sys.argv)!=2:
  print("Run program with command line argument for input file name")
  sys.exit(255)


with open(sys.argv[1],'r') as f:
  trees = [[int(num) for num in line.strip()] for line in f]

visible = [[False for _ in t] for t in trees]
size = len(trees)

# check from left
for row in range(size):
  height=-1
  for col in range(size):
    if trees[row][col]>height:
      height=trees[row][col]
      visible[row][col]=True

# check from right
for row in range(size):
  height=-1
  for col in range(size-1,-1,-1):
    if trees[row][col]>height:
      height=trees[row][col]
      visible[row][col]=True

# check from top
for col in range(size):
  height=-1
  for row in range(size):
    if trees[row][col]>height:
      height=trees[row][col]
      visible[row][col]=True


# check from right
for col in range(size):
  height=-1
  for row in range(size-1,-1,-1):
    if trees[row][col]>height:
      height=trees[row][col]
      visible[row][col]=True

print("Number of visible trees from the outside:")
print(sum([sum(row) for row in visible]))

# find most scenic tree
scores = [[0 for _ in t] for t in trees]
for row in range(size):
  for col in range(size):
    height=trees[row][col]
    score=1
    
    #check to left
    distance=0
    while(col-distance>=1 and (distance ==0 or trees[row][col-distance]<height)):
      distance=distance+1
    score=score*distance
    
    #check to right
    distance=0
    while(col+distance<=size-2 and (distance ==0 or trees[row][col+distance]<height)):
      distance=distance+1
    score=score*distance
    
    #check to top
    distance=0
    while(row-distance>=1 and (distance ==0 or trees[row-distance][col]<height)):
      distance=distance+1
    score=score*distance
    
    #check to bottow
    distance=0
    while(row+distance<=size-2 and (distance ==0 or trees[row+distance][col]<height)):
      distance=distance+1
    score=score*distance
    
    scores[row][col]=score

print("Score of most scenic tree:")
print(max([max(row) for row in scores]))

