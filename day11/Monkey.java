import java.util.LinkedList;
import java.util.ArrayList;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import java.util.Arrays;

public class Monkey {
  public enum Operator {PLUS, MINUS, MULT, DIV}
  
  public static ArrayList<Monkey> monkeys=new ArrayList<Monkey>();
  public static int worry_reducer=3;
  
  public static void performRound() {
    for(Monkey m : monkeys)
      m.turn();
  }
  
  public static void printAllItems() {
    for(int i=0;i<monkeys.size();i++) {
      System.out.println("Monkey "+i+": ");
      System.out.print("  ");
      monkeys.get(i).printItems();
    }
  }
  
  public static int monkeyBusiness() {
    int[] inspected=new int[monkeys.size()];
    for(int i=0;i<monkeys.size();i++) 
      inspected[i]=monkeys.get(i).getInspectedCount();
    Arrays.sort(inspected);
    return inspected[monkeys.size()-2]*inspected[monkeys.size()-1];
  }
  
  public static Monkey getMonkeyFromString(String s) {
    String[] line = s.split(System.lineSeparator());
    if(line.length<6)
      throw new RuntimeException("Cannot parse Monkey input. ");

    int lc=0;
    while(line[lc].length()==0)
      lc++;
    
    final Pattern pattern = Pattern.compile("^Monkey (\\d+):$", Pattern.CASE_INSENSITIVE);
    final Matcher matcher = pattern.matcher(line[lc]);
    if(!matcher.find())
      throw new RuntimeException("Cannot parse Monkey input. ");
    if(Integer.parseInt(matcher.group(1))!=monkeys.size())
      throw new RuntimeException("Cannot parse Monkey input. ");
      
    final Pattern patternList = Pattern.compile("Starting items: ([\\d, ]+)$", Pattern.CASE_INSENSITIVE);
    final Matcher matcherList = patternList.matcher(line[lc+1]);
    if(!matcherList.find())
      throw new RuntimeException("Cannot parse Monkey input. ");
    String startlist = matcherList.group(1);
      
    final Pattern patternOperation = Pattern.compile("Operation: new = old ([\\+\\-\\*\\/]) (\\d+|old)$", Pattern.CASE_INSENSITIVE);
    final Matcher matcherOperation = patternOperation.matcher(line[lc+2]);
    if(!matcherOperation.find())
      throw new RuntimeException("Cannot parse Monkey input. ");
    Operator operator=Operator.PLUS;
    switch(matcherOperation.group(1)){
      case "+":
        operator=Monkey.Operator.PLUS;
        break;
      case "-":
        operator=Monkey.Operator.MINUS;
        break;
      case "*":
        operator=Monkey.Operator.MULT;
        break;
      case "/":
        operator=Monkey.Operator.DIV;
        break;
    }
    boolean reuse= false;
    int operant=0;
    if(matcherOperation.group(2).equals("old"))
      reuse=true;
    else
      operant = Integer.parseInt(matcherOperation.group(2));
    
    final Pattern patternTest = Pattern.compile("Test: divisible by ([\\d, ]+)$", Pattern.CASE_INSENSITIVE);
    final Matcher matcherTest = patternTest.matcher(line[lc+3]);
    if(!matcherTest.find())
      throw new RuntimeException("Cannot parse Monkey input. ");
    int test_modulo = Integer.parseInt(matcherTest.group(1));
    
    final Pattern patternTestTrue = Pattern.compile("If true: throw to monkey ([\\d, ]+)$", Pattern.CASE_INSENSITIVE);
    final Matcher matcherTestTrue = patternTestTrue.matcher(line[lc+4]);
    if(!matcherTestTrue.find())
      throw new RuntimeException("Cannot parse Monkey input. ");
    int test_true = Integer.parseInt(matcherTestTrue.group(1));
    
    final Pattern patternTestFalse = Pattern.compile("If false: throw to monkey ([\\d, ]+)$", Pattern.CASE_INSENSITIVE);
    final Matcher matcherTestFalse = patternTestFalse.matcher(line[lc+5]);
    if(!matcherTestFalse.find())
      throw new RuntimeException("Cannot parse Monkey input. ");
    int test_false = Integer.parseInt(matcherTestFalse.group(1));
    
    
    Monkey m = new Monkey(operator,reuse,operant,test_modulo,test_true,test_false);
    m.setItemList(startlist.split(","));
    
    return m;
  }
  
  private final Operator operator;
  private final int operant;
  private final boolean reuse;
  private final int test_modulo;
  private final int test_true;
  private final int test_false;
  
  public Monkey(Monkey.Operator op, boolean valisitem, int val, int mod, int tr, int fal) {
    operator=op;
    reuse=valisitem;
    operant=val;
    test_modulo=mod;
    test_true=tr;
    test_false=fal;
    
    items=new LinkedList<Long>();
    
    monkeys.add(this);
  }
  
  private LinkedList<Long> items;
  private int inspected = 0;
  
  public void appendItem(long item) {
    items.addLast(item);
  }
  
  public void printItems() {
    System.out.print("Items: ");
    for (long i : items) {
      System.out.print(i);
      System.out.print(" ");
    }
    System.out.println();
  }
  
  public void setItemList(String...elements) {
    items.clear();
    for(String i : elements)
      items.add(Long.parseLong(i.trim()));
  }
  
  public void setItemList(long...elements) {
    items.clear();
    for(long i : elements)
      items.add(i);
  }
  
  public void inspectItem() {
    inspected++;
    long item = items.removeLast();
    long op=operant;
    if(reuse)
      op=item;
    switch(operator) {
      case PLUS:
        item+=op;
        break;
      case MINUS:
        item-=op;
        break;
      case MULT:
        item*=op;
        break;
      case DIV:
        item/=op;
        break;
    }
    
    item/=worry_reducer;
    if(item%test_modulo==0)
      Monkey.monkeys.get(test_true).appendItem(item);
    else
      Monkey.monkeys.get(test_false).appendItem(item);
  }
  
  public int getInspectedCount(){
    return inspected;
  }
  
  public void turn() {
    while(!items.isEmpty())
      inspectItem();
  }


}
