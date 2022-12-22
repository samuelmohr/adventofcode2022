package AdventOfCode;

import java.io.FileInputStream;
import java.io.File;
import java.io.BufferedReader;
import java.io.InputStreamReader;



public class Numberlist {

  protected class ListElement {
    private long number;
    private ListElement prev,next,following;
    
    public ListElement(long n) {
      number=n;
    }
    
    public void setPrev(ListElement le) {prev=le;}
    public void setNext(ListElement le) {next=le;}
    public void setFollower(ListElement le) {following=le;}
    public ListElement getFollower() {return following;}
    public ListElement getNext() {return next;}
    public ListElement getPrev() {return prev;}
    public void updateValue(long n) {number=n;}
    
    public long get(){return number;}
    public long get(int pos) {
      if(pos>0)
        return next.get(pos-1);
      else if (pos==0)
        return number;
      else 
        throw new RuntimeException("Expect positive position");
    }
    public ListElement find(long nb) {
      if(nb==number)
        return this;
      return next.find(nb);
    }
    
    public void shiftByNumber(long number,int listlength) {
      int offset=((int)(number%(listlength-1)));
      if(offset==0)
        return;
      prev.setNext(next);
      next.setPrev(prev);
      shift(offset-(offset<0?1:0),this);
    }
     
    public void shiftByNumber(int number) {
      if(number==0)
        return;
      prev.setNext(next);
      next.setPrev(prev);
      shift(number-(number<0?1:0),this);
    }
     
    protected void shift(int steps, ListElement le) {
      if(steps==0) {
        // place in
        le.setPrev(this);
        le.setNext(next);
        next.setPrev(le);
        setNext(le);
      } else if(steps>0)
        next.shift(steps-1,le);
      else if(steps<0)
        prev.shift(steps+1,le);
    } 
  
  }
  
  private ListElement first;
  private int length=0;
  
  public void add(int nb) {
    if(first==null) {
      first=new ListElement(nb);
      first.setNext(first);
      first.setPrev(first);
    } else {
      ListElement last=first.getPrev();
      ListElement tmp=new ListElement(nb);
      last.setNext(tmp);
      last.setFollower(tmp);
      tmp.setPrev(last);
      last=tmp;
    
      last.setNext(first);
      first.setPrev(last);
    }
    length++;
  }
  
  public long get(int pos) {
    return first.get(pos-1);
  }
  public long getFromElement(int startelement, int pos) {
    ListElement le=first.find(startelement);
    return le.get(pos);
  }

  public Numberlist() {
  }
  
  public void print() {
    ListElement le=first;
    while(le!=first.getPrev()) {
      System.out.print(le.get());
      System.out.print(", ");
      le=le.getNext();
    }
    System.out.print(le.get());
    System.out.println();
  }
  
  public void actionPart1() {
    ListElement active=first;
    while(active!=null) {
      active.shiftByNumber(active.get(),length);
      active=active.getFollower();
    }
  }
  
  public void multiplyAll(long val) {
    ListElement active=first;
    while(active!=null) {
      active.updateValue(active.get()*val);
      active=active.getFollower();
    }
  }
  

  public static Numberlist parse(String filename) {
    Numberlist list=new Numberlist();
    
    try{
      FileInputStream fis;
      BufferedReader reader;

      File file = new File(filename);
      fis = new FileInputStream(file);
      reader=new BufferedReader(new InputStreamReader(fis));
      
      String line = reader.readLine();
      while (line != null) {
        list.add(Integer.parseInt(line));
        line = reader.readLine();
      }
      
      reader.close();
    }catch(Throwable e){
      System.out.println("Cannot read file");
      System.exit(-1);
    }
    return list;
  }
  
  public static void main(String[] args) {
    if(args.length!=1) {
      System.out.println("Use program with input file as runtime argument.");
      System.exit(255);
    }
    
    Numberlist list=parse(args[0]);
    
    list.actionPart1();
    
    int e1=(int)list.getFromElement(0,1000);
    int e2=(int)list.getFromElement(0,2000);
    int e3=(int)list.getFromElement(0,3000);
    System.out.println("The three coordinates are:");
    System.out.println(e1);
    System.out.println(e2);
    System.out.println(e3);

    System.out.println("The sum is:");
    System.out.println(e1+e2+e3);
    
    // PART 2
    System.out.println();
    System.out.println("PART 2:");
    list=parse(args[0]);

    list.multiplyAll(811589153);
    
    for(int i=0;i<10;i++) {
      list.actionPart1();
    }
    
    long le1=list.getFromElement(0,1000);
    long le2=list.getFromElement(0,2000);
    long le3=list.getFromElement(0,3000);
    System.out.println("The three coordinates are:");
    System.out.println(le1);
    System.out.println(le2);
    System.out.println(le3);

    System.out.println("The sum is:");
    System.out.println(le1+le2+le3);
    
    System.exit(0);
  }
}
