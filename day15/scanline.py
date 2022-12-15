
def getBeacons(beacons,rowOfInterest):
  return sorted(list(set([b[0] for b in beacons if b[1]==rowOfInterest])))

def getOverlaps(sensors,beacons,rowOfInterest):
  cover_in_rowOfInterest=[]
  for i in range(len(sensors)):
    distance=abs(beacons[i][0]-sensors[i][0])+abs(beacons[i][1]-sensors[i][1])
    delta=distance-abs(sensors[i][1]-rowOfInterest)
    if delta<0:
      continue
    cover_in_rowOfInterest.append((sensors[i][0]-delta,sensors[i][0]+delta))

  return sorted(cover_in_rowOfInterest)


def analyseRow(sensors,beacons,row):
  cover_in_rowOfInterest=getOverlaps(sensors,beacons,row)
  
  marker=cover_in_rowOfInterest[0][0]
  end_list=[cover_in_rowOfInterest[0][1]]
  next_cover_it=1
  
  startstoplist=[marker]
  while next_cover_it<len(cover_in_rowOfInterest) or len(end_list)!=0:
    if len(end_list)==0 or (next_cover_it<len(cover_in_rowOfInterest) and cover_in_rowOfInterest[next_cover_it][0]<=end_list[0]):
      marker=cover_in_rowOfInterest[next_cover_it][0]
      if len(end_list)==0:
        startstoplist.append(marker)
      end_list.append(cover_in_rowOfInterest[next_cover_it][1])
      next_cover_it+=1
      end_list=sorted(end_list)
    else:
      marker=end_list.pop(0)
      if len(end_list)==0:
        startstoplist.append(marker)
  
  return startstoplist


def getLineFreeOfBeacons(sensors,beacons,row):
  fob_list=analyseRow(sensors,beacons,row)
  
  # clean from known beacons
  for b in getBeacons(beacons,row):
    i=0
    while i<len(fob_list) and fob_list[i]<b:
      i=i+1
    
    if i%2==0:
      continue
      
    fob_list.insert(i,b+1)
    fob_list.insert(i,b-1)
  
  return fob_list



def lenOfFOBrange(fob_list,range_min,range_max):
  val=0
  for i in range(int(len(fob_list)/2)):
    if fob_list[2*i+1]<range_min:
      continue
    if fob_list[2*i]>range_max:
      continue
    val+=min(range_max,fob_list[2*i+1])-max(range_min,fob_list[2*i])+1
    
  return val

def lenOfFOB(fob_list):
  return lenOfFOBrange(fob_list,fob_list[0],fob_list[-1])


def spotsInFOB(fob_list,range_min,range_max):
  val=[]
  i=0
  while i+1<len(fob_list) and fob_list[i+1]<range_min:
    i+=1
  
  v=range_min
  while v<=range_max:
    if (i>=len(fob_list) or v<fob_list[i]) and i%2==0:
      val.append(v)
    while i<len(fob_list) and fob_list[i]<=v:
      i+=1
    if i%2==1:
      v=fob_list[i]
    else:
      v+=1
  
  return val  

