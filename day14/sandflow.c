#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READBUFFER 1024

#define DIMENSION_LEFT 000
#define DIMENSION_RIGHT 1000
#define DIMENSION_DEPTH 200

const char air = '.';
const char sand = 'o';
const char rock = '#';
const char sandflow = '~';
const char start = '+';
const char bottom = '_';

char space[DIMENSION_RIGHT-DIMENSION_LEFT+1][DIMENSION_DEPTH];

int used_left = 500;
int used_right = 500;
int used_depth = 0;

int start_x = 500;
int start_y = 0;

void printSpace() {
  for(int j=0;j<DIMENSION_DEPTH;j++) {
    if(j>used_depth+2)
      return;
    for(int i=0;i<=DIMENSION_RIGHT-DIMENSION_LEFT;i++) {
      if(i<used_left-DIMENSION_LEFT-3 || i>used_right-DIMENSION_LEFT+3)
        continue;
      
      printf("%c",space[i][j]);
    }
    printf("%c",'\n');
  }
}

int sign(int x) {
    return (x > 0) - (x < 0);
}

void mark_line(int sx, int sy, int ex, int ey) {
  printf("Print line from (%d,%d) to (%d,%d)\n",sx,sy,ex,ey);
  // some checks and canvas updates
  if(sx<DIMENSION_LEFT || sx>DIMENSION_RIGHT) {
        printf("horizontal dimension is too small!");
        exit(160);
  } else {
    used_left=used_left<sx?used_left:sx;
    used_right=used_right>sx?used_right:sx;
  }
  if(ex<DIMENSION_LEFT || ex>DIMENSION_RIGHT) {
        printf("horizontal dimension is too small!");
        exit(160);
  } else {
    used_left=used_left<ex?used_left:ex;
    used_right=used_right>ex?used_right:ex;
  }
  if(sy<0 || sy>DIMENSION_DEPTH) {
        printf("vertical dimension is too small!");
        exit(161);
  } else {
    used_depth=used_depth>sy?used_depth:sy;
  }
  if(ey<0 || ey>DIMENSION_DEPTH) {
        printf("vertical dimension is too small!");
        exit(161);
  } else {
    used_depth=used_depth>ey?used_depth:ey;
  }

  if(sx==ex) {
    for(int i=sy;i!=ey;i+=sign(ey-sy))
      space[sx-DIMENSION_LEFT][i]=rock;
    space[ex-DIMENSION_LEFT][ey]=rock;
  } else if(sy==ey) {
    for(int i=sx-DIMENSION_LEFT;i!=ex-DIMENSION_LEFT;i+=sign(ex-sx))
      space[i][sy]=rock;
    space[ex-DIMENSION_LEFT][ey]=rock;
  } else {
          printf("Formation error, rocks have to be in line\n");
          exit(63);
  }
}

void setGround() {
  for(int i=0;i<=DIMENSION_RIGHT-DIMENSION_LEFT;i++)
    space[i][used_depth+2]=bottom;
  used_depth+=2;
}

int setStart(int sx, int sy) {
  if(sx<DIMENSION_LEFT || sx>DIMENSION_RIGHT) {
        printf("horizontal dimension is too small!");
        exit(160);
  } else {
    used_left=used_left<sx?used_left:sx;
    used_right=used_right>sx?used_right:sx;
  }
  if(sy<0 || sy>DIMENSION_DEPTH) {
        printf("vertical dimension is too small!");
        exit(161);
  } else {
    used_depth=used_depth>sy?used_depth:sy;
  }

  start_x=sx;
  start_y=sy;
  
  space[start_x-DIMENSION_LEFT][start_y]=start;
}

int sandunit() {
  int pos=start_x-DIMENSION_LEFT;
  for(int d=start_y+1;d<=used_depth;d++) {
    // d is depth
    if(space[pos][d]==air || space[pos][d]==sandflow)
      continue;
    
    if(pos!=0 && (space[pos-1][d]==air || space[pos-1][d]==sandflow)) {
      pos--;
      continue;
    }
    if(pos+DIMENSION_LEFT!=DIMENSION_RIGHT && (space[pos+1][d]==air || space[pos+1][d]==sandflow)) {
      pos++;
      continue;
    }
    
    // else got rested
    if(d>start_y+1) {
      space[pos][d-1]=sand;
      return 0;
    }
    
    printf("sand source clogged!\n");
    return -1;
  }
  // infinite flow
  return 1;
}

void trace_sand() {
  int pos=start_x-DIMENSION_LEFT;
  for(int d=start_y+1;d<=used_depth;d++) {
    // d is depth
    if(space[pos][d]==air) {
      space[pos][d]=sandflow;
      continue;
    }
    
    if(pos!=0 && space[pos-1][d]==air) {
      pos--;
      space[pos][d]=sandflow;
      continue;
    }
    if(pos+DIMENSION_LEFT!=DIMENSION_RIGHT && space[pos+1][d]==air) {
      pos++;
      space[pos][d]=sandflow;
      continue;
    }
    
    // else got rested
    if(d>start_y+1) {
      space[pos][d-1]=sand;
      return;
    }
    
    printf("sand source clogged!\n");
    return;
  }
  // infinite flow
  space[pos][used_depth+1]=sandflow;
  space[pos][used_depth+2]=sandflow;
}

int main()
{
    // clear array
    char* ptr = &space[0];
    for(int j=0;j<DIMENSION_DEPTH*(DIMENSION_RIGHT-DIMENSION_LEFT+1);++j,++ptr) 
      *ptr=air;
    
    char* buffer = (char *)malloc(sizeof(char) * READBUFFER);
    size_t characters=READBUFFER;
    ssize_t read;

    while ((read=getline(&buffer,&characters,stdin))!=-1){
      if(read>=READBUFFER) {
        printf("READBUFFER is too small!");
        exit(150);
      }
      
      int x,y;
      char* pos=buffer;
      char* end;
      x=strtol(pos,&end,10);
      if(end==NULL || *end!=',') {
        printf("Format error\n");
        exit(64);
      }
      pos=end+1;
      y=strtol(pos,&end,10);
      while(end-buffer<read-1){
        int a,b;
        
        if(end==NULL || *end!=' ' || *++end!='-' || *++end!='>' || *++end!=' ') {
          printf("Format error\n");
          printf("got %c",*end);
          exit(64);
        }
        pos=end+1;
        a=strtol(pos,&end,10);
        if(end==NULL || *end!=',') {
          printf("Format error\n");
          exit(64);
        }
        pos=end+1;
        b=strtol(pos,&end,10);
        
        // mark rock line
        mark_line(x,y,a,b);
        x=a;
        y=b;
      }
    }
    free(buffer);
    
    setStart(500,0);
        
    // perform action
    printSpace();
    printf("That was the start rock formation. Let's run sand. \n");
    
    int count=0;
    while(! sandunit())
      count++;
    
    trace_sand();
    printSpace();
    
    printf("A total of %d units of sand rested. \n",count);
    
    // PART 2
    printf("\n\nPART 2\n");
    
    setGround();
    printSpace();
    printf("That was the start rock formation with ground. Let's run more sand. \n");

    while(! sandunit())
      count++;
    
    // +1 for the source
    count++;
    printSpace();
    
    printf("A total of %d units of sand rested. \n",count);
    exit(0);
}
