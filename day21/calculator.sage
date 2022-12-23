import sys
import numpy as np
import numbers

import re

if len(sys.argv)!=2:
  print("Run program with command line argument for input file name")
  sys.exit(255)

monkeys={}

# read

prog = re.compile("^(\w{4}): (.+)$")
with open(sys.argv[1],'r') as f:
  for line in f:
    result = prog.match(line)
    if result==None:
          sys.exit(255)
    monkeys[result.group(1)]=result.group(2)

#print(monkeys)

x=var('x')

# PART 1
number_prog = re.compile("^(\d+|x)$")
operation_prog = re.compile("^(\w{4}) (\+|\-|\*|\/|\=) (\w{4})$")
def calculate(monkey):
    string = monkeys[monkey]
    if string==None:
        sys.exit(128)
    result = number_prog.match(string)
    if result!=None:
        try:
            return int(result.group(1))
        except ValueError:
            return x
    result = operation_prog.match(string)
    if result==None:
        sys.exit(128)
    op1=calculate(result.group(1))
    op2=calculate(result.group(3))
    if(result.group(2)=='+'):
        return op1+op2
    if(result.group(2)=='-'):
        return op1-op2
    if(result.group(2)=='*'):
        return op1*op2
    if(result.group(2)=='/'):
        return op1/op2
    if(result.group(2)=='='):
        return op1 == op2
    sys.exit(127)
    

start="root"
print("Result of Monkey Math:")
print(calculate(start))

# part 2
print("\nPART 2")

monkeys["root"] = monkeys["root"].replace('+','=').replace('-','=').replace('*','=').replace('/','=')
monkeys["humn"] = 'x'
print("Your call for Monkey Equation:")
print(solve([calculate("root")],x))
