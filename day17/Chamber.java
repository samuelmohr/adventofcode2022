package AdventOfCode;

import java.io.FileInputStream;
import java.io.File;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.InputStream;


import java.util.Arrays;
import java.util.ArrayList;
import java.util.HashMap;

abstract class Rock {
  protected int x_pos;
  protected int y_pos;
  private int delta_d=0;
  private int delta_l=0;
  private int delta_r=0;
  public Rock(int x,int y) {x_pos=x;y_pos=y;}
  
  public boolean canMoveDown(Chamber ch) {return checkPosition(ch,x_pos,y_pos-1);}
  public boolean canMoveLeft(Chamber ch) {return checkPosition(ch,x_pos-1,y_pos);}
  public boolean canMoveRight(Chamber ch) {return checkPosition(ch,x_pos+1,y_pos);}
  protected abstract boolean checkPosition(Chamber ch,int x,int y);
  
  public void moveDown() {y_pos--;delta_d++;}
  public void moveLeft() {x_pos--;delta_l++;}
  public void moveRight() {x_pos++;delta_r++;}
  
  public abstract int type_code();
  public int hash() {
    int val=type_code();
    val = val<<8;
    final int bitmask = 0x000F;
    val+=delta_d&bitmask;
    val = val<<8;
    val+=delta_l&bitmask;
    val = val<<8;
    val+=delta_r&bitmask;
    return val;
  }
  
  public abstract void fix(Chamber ch);
}

class DashRock extends Rock {
  public DashRock(int x,int y) {super(x,y);}
  public int type_code() {return 0;}
  protected boolean checkPosition(Chamber ch,int x,int y) {
    return ch.free(x,y) && ch.free(x+1,y) && ch.free(x+2,y) && ch.free(x+3,y);
  }
  public void fix(Chamber ch) {
    ch.take(x_pos,y_pos);
    ch.take(x_pos+1,y_pos);
    ch.take(x_pos+2,y_pos);
    ch.take(x_pos+3,y_pos);
  }
}

class LineRock extends Rock {
  public LineRock(int x,int y) {super(x,y+3);}
  public int type_code() {return 1;}
  protected boolean checkPosition(Chamber ch,int x,int y) {
    return ch.free(x,y) && ch.free(x,y-1) && ch.free(x,y-2) && ch.free(x,y-3);
  }
  public void fix(Chamber ch) {
    ch.take(x_pos,y_pos);
    ch.take(x_pos,y_pos-1);
    ch.take(x_pos,y_pos-2);
    ch.take(x_pos,y_pos-3);
  }
}

class BlockRock extends Rock {
  public BlockRock(int x,int y) {super(x,y+1);}
  public int type_code() {return 2;}
  protected boolean checkPosition(Chamber ch,int x,int y) {
    return ch.free(x,y) && ch.free(x+1,y) && ch.free(x,y-1) && ch.free(x+1,y-1);
  }
  public void fix(Chamber ch) {
    ch.take(x_pos,y_pos);
    ch.take(x_pos+1,y_pos);
    ch.take(x_pos,y_pos-1);
    ch.take(x_pos+1,y_pos-1);
  }
}

class PlusRock extends Rock {
  public PlusRock(int x,int y) {super(x,y+2);}
  public int type_code() {return 3;}
  protected boolean checkPosition(Chamber ch,int x,int y) {
    return ch.free(x,y-1) && ch.free(x+1,y) && ch.free(x+1,y-1) && ch.free(x+2,y-1) && ch.free(x+1,y-2);
  }
  public void fix(Chamber ch) {
    ch.take(x_pos,y_pos-1);
    ch.take(x_pos+1,y_pos);
    ch.take(x_pos+1,y_pos-1);
    ch.take(x_pos+2,y_pos-1);
    ch.take(x_pos+1,y_pos-2);
  }
}

class LRock extends Rock {
  public LRock(int x,int y) {super(x,y+2);}
  public int type_code() {return 4;}
  protected boolean checkPosition(Chamber ch,int x,int y) {
    return ch.free(x,y-2) && ch.free(x+1,y-2) && ch.free(x+2,y-2) && ch.free(x+2,y-1) && ch.free(x+2,y);
  }
  public void fix(Chamber ch) {
    ch.take(x_pos,y_pos-2);
    ch.take(x_pos+1,y_pos-2);
    ch.take(x_pos+2,y_pos-2);
    ch.take(x_pos+2,y_pos-1);
    ch.take(x_pos+2,y_pos);
  }
}



public class Chamber {
  private final int wide;
  private ArrayList<Boolean[]> space;
  private int height=0;
  private long offset_height=0;
  
  public Chamber(int w) {
    wide=w;
    space=new ArrayList<Boolean[]>();
    fill();
  }
  
  public int howTall() {return height;}
  public long totalHeight() {return height+offset_height;}
  public void setOffsetHeight(long h) {offset_height+=h;}
  
  private void fill(){
    for(int i=space.size();i<height+10;i++) {
      Boolean[] array=new Boolean[wide];
      Arrays.fill(array,false);
      space.add(array);
    }
  }
  
  public boolean free(int x,int y) {
    if(y<0 || x<0 || x>=wide)
      return false;
    return !space.get(y)[x];
  }
  
  public void take(int x,int y) {
    if(y<0 || x<0 || x>=wide)
      throw new RuntimeException("out of bound");
    space.get(y)[x]=true;
    height=Math.max(height,y+1);
    fill();
  }
  
  public long hash() {
    long val=0;
    int i=0;
    out:
    for(Boolean[] row : space)
      for(boolean cell : row) {
        if(cell)
          val++;
        if(i<32){
          i++;
          val <<= 1;
        } else
          break out;
      }
    if(i<32)
      val <<= (32-i);
    return val;
  }
  
  public void draw() {
    for(int i=height+3;i>=0;i--) {
      System.out.print('|');
      for(int j=0;j<wide;j++)
        if(space.get(i)[j])
          System.out.print('#');
        else
          System.out.print('.');
      System.out.print('|');
      System.out.println();
    }
      System.out.print('+');
      for(int j=0;j<wide;j++)
          System.out.print('-');
      System.out.print('+');
      System.out.println();
  }



  public static Rock spawn(Chamber ch, Class type, int rel_x, int rel_y) {
    if(type.getSuperclass() != Rock.class)
      throw new RuntimeException("type should be a rock");
    
    int y=ch.howTall()+rel_y;
    Rock rock=null;
    try {
      rock =(Rock)type.getDeclaredConstructor(int.class,int.class).newInstance(rel_x,y);
    }catch(Throwable e){
        System.out.println("Rock construction failed");
        System.exit(-1);
    }
    return rock;
  }
  
  public static boolean fallDown(Chamber ch, Rock r) {
    boolean suc=r.canMoveDown(ch);
    if(suc)
      r.moveDown();
    return suc;
  }

  public static boolean blowLeft(Chamber ch, Rock r) {
    boolean suc=r.canMoveLeft(ch);
    if(suc)
      r.moveLeft();
    return suc;
  }

  public static boolean blowRight(Chamber ch, Rock r) {
    boolean suc=r.canMoveRight(ch);
    if(suc)
      r.moveRight();
    return suc;
  }
  
  public static void restRock(Chamber ch, Rock r) {
    r.fix(ch);
  }


  public static void main(String[] args) {
    if(args.length!=1) {
      System.out.println("Use program with input file as runtime argument.");
      System.exit(255);
    }
    
    long rounds=2022;
    System.out.println("Height of Chamber after "+rounds+": "+action(args[0],rounds));
    rounds=1000000000000L;
    System.out.println("Height of Chamber after "+rounds+": "+action(args[0],rounds));
    System.exit(0);
  }
  
  public static long action(String filename, long rounds) {
   try{
    FileInputStream fis;
    Reader wind;

      File file = new File(filename);
      fis = new FileInputStream(file);
      wind=new BufferedReader(new InputStreamReader(fis));
    
    final int wide=7;
    Chamber chamber=new Chamber(wide);
    for(long i=0;i<rounds;i++){
      Class type=null;
      switch((int)(i%5)) {
        case 0:
          type=DashRock.class;
          break;
        case 1:
          type=PlusRock.class;
          break;
        case 2:
          type=LRock.class;
          break;
        case 3:
          type=LineRock.class;
          break;
        case 4:
          type=BlockRock.class;
          break;
      }
      
      Rock r=spawn(chamber,type,2,3);
      
     while(true) {
      // blow
      int read=wind.read();
      if(read==-1){
        fis.getChannel().position(0);
        wind=new BufferedReader(new InputStreamReader(fis));
        read=wind.read();
      }
      char c=(char) read;
      if(c=='<')
        blowLeft(chamber,r);
      else if(c=='>')
        blowRight(chamber,r);
      else if(c=='\n') {
      // the next line is to optimise for part 2. 
        i=hashStatus(i,rounds,chamber,r);
        continue;
      }else
        throw new RuntimeException("unrecognised character: "+c);
      
      // fall
      if(!fallDown(chamber,r)) {
        restRock(chamber,r);
        break;
      }
     }
    }
//    chamber.draw();
//    System.out.println();
    fis.close();
    hashes.clear();

    return chamber.totalHeight();

    }catch(Throwable e){
      System.out.println(e);
      e.printStackTrace();
      System.out.println("Cannot read file");
      System.exit(-1);
    }
    return -1;
  }
  
  private static HashMap<Long,Long[]> hashes=new HashMap<Long,Long[]>();
  public static long hashStatus(long current,long maxnumber,Chamber chamber,Rock r) {
    long hash=chamber.hash();
    hash <<= 32;
    hash+=r.hash();
//    System.out.println("INFO: build hash at "+Long.toString(current)+" of Rock of Type "+Integer.toString(r.type_code())+" gives hash: "+Long.toString(hash));
    if(hashes.containsKey(hash)) {
      Long[] ret=hashes.get(hash);
      long rounds=(maxnumber-ret[0])/(current-ret[0])-1;
      chamber.setOffsetHeight(rounds*(chamber.howTall()-ret[1]));
      hashes.clear();
      return current+rounds*(current-ret[0]);
    } else {
      Long ary[]={current,(long)chamber.howTall()};
      hashes.put(hash,ary);
      return current;
    }
  }
}
