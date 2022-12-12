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
    try:
      if(alphabet_index(nodes[i+1][j]) - alphabet_index(letter) == 1):
        G.add_edge((i,j),(i+1,j))
      if(alphabet_index(nodes[i+1][j]) - alphabet_index(letter) == -1):
        G.add_edge((i+1,j),(i,j))
      if(alphabet_index(nodes[i+1][j]) - alphabet_index(letter) == 0):
        G.add_edge((i+1,j),(i,j))
        G.add_edge((i,j),(i+1,j))
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
    try:
      if(alphabet_index(nodes[i][j+1]) - alphabet_index(letter) == 1):
        G.add_edge((i,j),(i,j+1))
      if(alphabet_index(nodes[i][j+1]) - alphabet_index(letter) == -1):
        G.add_edge((i,j+1),(i,j))
      if(alphabet_index(nodes[i][j+1]) - alphabet_index(letter) == 0):
        G.add_edge((i,j+1),(i,j))
        G.add_edge((i,j),(i,j+1))
    except IndexError:
      pass

shortest_path_v=G.shortest_path(start,end)
shortest_path=[(a,b) for a,b in zip(shortest_path_v,shortest_path_v[1:])]
plot=G.plot(vertex_labels=lambda v : str(nodes[v[0]][v[1]]),edge_thickness=.1,vertex_size=200,edge_color="gray",edge_colors={"green":shortest_path},pos=pos_dict,vertex_color="red")
save(plot,'Graph.png')
# shortest path
print("Length of shortest path from 'S' to 'E' is "+str(len(shortest_path)))
os.system('display Graph.png')
#input("Press Enter to continue...")

