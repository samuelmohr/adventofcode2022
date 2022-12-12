#!/usr/bin/sage
import sys
import os

if len(sys.argv)!=2:
  print("Use program with input file as runtime argument.")
  sys.exit(255)

# read file to get char of nodes
with open(sys.argv[1]) as file:
  nodes=file.read().splitlines()
  
alphabet = ['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z']
def alphabet_index(char):
  if char=='S':
    char='a'
  if char=='E':
    char='z'
  return alphabet.index(char)
  
# build graph and add vertices
G=DiGraph()
pos_dict={}
for i in range(len(nodes)):
  for j in range(len(nodes[i])):
    G.add_vertex((i,j))
    pos_dict[(i,j)]=[j,-i]
    
# add edges to increasing and stationary
for i in range(len(nodes)):
  for j in range(len(nodes[i])):
    letter=nodes[i][j]
    if letter=='S':
      start=(i,j)
      letter='a'
    if letter=='E':
      end=(i,j)
      letter='z'
    if i!=0:
      if(alphabet_index(nodes[i-1][j]) - alphabet_index(letter) == 1):
        G.add_edge((i,j),(i-1,j))
      if(alphabet_index(nodes[i-1][j]) - alphabet_index(letter) == -1):
        G.add_edge((i-1,j),(i,j))
      if(alphabet_index(nodes[i-1][j]) - alphabet_index(letter) == 0):
        G.add_edge((i-1,j),(i,j))
        G.add_edge((i,j),(i-1,j))
      if(alphabet_index(nodes[i-1][j]) < alphabet_index(letter)):
        G.add_edge((i,j),(i-1,j))
      else:
        G.add_edge((i-1,j),(i,j))
    try:
      if(alphabet_index(nodes[i+1][j]) - alphabet_index(letter) == 1):
        G.add_edge((i,j),(i+1,j))
      if(alphabet_index(nodes[i+1][j]) - alphabet_index(letter) == -1):
        G.add_edge((i+1,j),(i,j))
      if(alphabet_index(nodes[i+1][j]) - alphabet_index(letter) == 0):
        G.add_edge((i+1,j),(i,j))
        G.add_edge((i,j),(i+1,j))
      if(alphabet_index(nodes[i+1][j]) < alphabet_index(letter)):
        G.add_edge((i,j),(i+1,j))
      else:
        G.add_edge((i+1,j),(i,j))
    except IndexError:
      pass
    if j!=0:
      if(alphabet_index(nodes[i][j-1]) - alphabet_index(letter) == 1):
        G.add_edge((i,j),(i,j-1))
      if(alphabet_index(nodes[i][j-1]) - alphabet_index(letter) == -1):
        G.add_edge((i,j-1),(i,j))
      if(alphabet_index(nodes[i][j-1]) - alphabet_index(letter) == 0):
        G.add_edge((i,j-1),(i,j))
        G.add_edge((i,j),(i,j-1))
      if(alphabet_index(nodes[i][j-1]) < alphabet_index(letter)):
        G.add_edge((i,j),(i,j-1))
      else:
        G.add_edge((i,j-1),(i,j))
    try:
      if(alphabet_index(nodes[i][j+1]) - alphabet_index(letter) == 1):
        G.add_edge((i,j),(i,j+1))
      if(alphabet_index(nodes[i][j+1]) - alphabet_index(letter) == -1):
        G.add_edge((i,j+1),(i,j))
      if(alphabet_index(nodes[i][j+1]) - alphabet_index(letter) == 0):
        G.add_edge((i,j+1),(i,j))
        G.add_edge((i,j),(i,j+1))
      if(alphabet_index(nodes[i][j+1]) < alphabet_index(letter)):
        G.add_edge((i,j),(i,j+1))
      else:
        G.add_edge((i,j+1),(i,j))
    except IndexError:
      pass

all_dist=G.distance_all_pairs()
# shortest path
answer=all_dist[start][end]
print("Length of shortest path from 'S' to 'E' is "+str(answer))
#os.system('display Graph.png')
input("Press Enter to continue...")

# part 2
for i in range(len(nodes)):
  for j in range(len(nodes[i])):
    if nodes[i][j]=='a':
      try:
        answer=min(answer,all_dist[(i,j)][end])
      except KeyError:
        pass
print("Length of shortest path from anywhere on level 'a' to 'E' is "+str(answer))


