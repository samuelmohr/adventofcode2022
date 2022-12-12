import java.io.FileInputStream;
import java.io.File;

public class Main {
  public static void main(String[] args) {
    if(args.length!=1) {
      System.out.println("Use program with input file as runtime argument.");
      System.exit(255);
    }
  
    String str="";
    try{
    File file = new File(args[0]);
    FileInputStream fis = new FileInputStream(file);
    byte[] data = new byte[(int) file.length()];
    fis.read(data);
    fis.close();
    
    str = new String(data, "UTF-8");
    }catch(Throwable e){
      System.out.println("Cannot read file");
      System.exit(-1);
    }
    
    String[] monkeys = str.split("(?m)^\\s*$");
    
    for(String s : monkeys)
      Monkey.getMonkeyFromString(s);
      
    for(int i=0;i<20;i++) {
      System.out.println("ROUND "+i);
      Monkey.printAllItems();
      Monkey.performRound();
    }
  
    System.out.println("AFTER 20 ROUNDs");
    Monkey.printAllItems();
    
    System.out.println();
    System.out.println("Monkey Business Coefficient: "+Monkey.monkeyBusiness());

    System.exit(0);
  }
}
