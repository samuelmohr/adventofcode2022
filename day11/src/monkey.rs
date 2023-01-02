extern crate num;

use lazy_static::lazy_static;
use regex::Regex;
use std::cell::RefCell;
use std::fmt::Display;
use std::str::FromStr;
use std::fmt::Debug;
use num::Integer;

pub struct Monkey<int>
where int: Integer + Display + From<i16> + FromStr + Copy, <int as FromStr>::Err:Debug
{
    op: Operator,
    m1: Operand<int>,
    m2: Operand<int>,
    policy: Jump,

    items: Vec<int>,
    inspected: u32,
}

pub struct Jump {
    module: i16,
    trueval: usize,
    falseval: usize,
}

pub enum Operator { Plus, Minus, Mult, Div }

pub enum Operand<int> {
    Terminal(int),
    Variable,
}

impl Jump {
    fn next<int: From<i16>+ Integer>(&self, val: int) -> usize {
        if val % <i16 as Into<int>>::into(self.module) != <i16 as Into<int>>::into(0) {
            self.falseval
        } else {
            self.trueval
        }
    }
}

impl<int> Monkey<int>
where int: Integer + Display + From<i16> + FromStr + Copy, <int as FromStr>::Err:Debug
{
    pub fn appendItem(&mut self, val: int) {
        self.items.push(val);
    }

    pub fn printItems(&self) {
        println!("Items: {}", self.items.iter().skip(1).fold(match self.items.first() {Some(x) => format!("{x}"), _ => String::new()}, |acc, &arg| format!("{acc}, {arg}")));
    }
    pub fn printMonkey(&self, number: u8) {
        print!("Monkey {number}: \n  ");
        self.printItems();
    }

    pub fn printInspectedCount(&self, number: u8) {
        print!(" Monkey {number} inspected items {} times \n  ",self.inspected);
    }

    pub fn inspectAll(&mut self, monkeylist: &[RefCell<Monkey<int>>], worry_reducer: i16, modulo_cap: Option<int>) {
        for i in &self.items {
            self.inspected += 1;
            let var1 = match &self.m1 {
                Operand::Terminal(x) => *x,
                Operand::Variable => *i
            };
            let var2 = match &self.m2 {
                Operand::Terminal(x) => *x,
                Operand::Variable => *i
            };

            let mut val: int = match &self.op {
                Operator::Plus => var1 + var2,
                Operator::Minus => var1 - var2,
                Operator::Mult => var1 * var2,
                Operator::Div => var1 / var2,
            };

            val = val/ <i16 as Into<int>>::into(worry_reducer);
            val = match modulo_cap {
                Some(x) => val%x, None=> val   };
            let next: usize = self.policy.next(val).into();
            monkeylist[next].borrow_mut().appendItem(val);
        }
        self.items.clear();
    }

    pub fn getInspectedCount(&self) -> u32 {
        self.inspected
    }

    pub fn getDecisionDivisor(&self) -> i16 {
        self.policy.module
    }

    pub fn new(s: &str) -> (usize, Monkey<int>) {
        let mut it = s.lines();

        lazy_static! {
        static ref regex_number: Regex = Regex::new("^Monkey (\\d+):$").unwrap();
        static ref regex_list: Regex = Regex::new("Starting items: ([\\d, ]+)$").unwrap();
        static ref regex_operation: Regex = Regex::new("Operation: new = (\\d+|old) ([\\+\\-\\*/]) (\\d+|old)$").unwrap();

        static ref regex_test: Regex = Regex::new("Test: divisible by ([\\d, ]+)$").unwrap();
        static ref regex_true: Regex = Regex::new("If true: throw to monkey ([\\d, ]+)$").unwrap();
        static ref regex_false: Regex = Regex::new("If false: throw to monkey ([\\d, ]+)$").unwrap();
    }
        let number: usize = regex_number.captures(it.next().expect("Too few lines of Monkey definition")).unwrap()[1].parse().unwrap();
        let start_list = &regex_list.captures(it.next().expect("Too few lines of Monkey definition")).unwrap()[1];

        let cap1 = &regex_operation.captures(it.next().expect("Too few lines of Monkey definition")).unwrap();
        let op: Operator = match &cap1[2] {
            "+" => Operator::Plus,
            "-" => Operator::Minus,
            "*" => Operator::Mult,
            "/" => Operator::Div,
            _ => panic!("impossible Operator")
        };
        let m1  = match &cap1[1] {
            "old" => Operand::Variable,
            s @ _ => Operand::Terminal(s.parse().unwrap())
        };
        let m2 = match &cap1[3] {
            "old" => Operand::Variable,
            s @ _ => Operand::Terminal(s.parse().unwrap())
        };

        let test: i16 = regex_test.captures(it.next().expect("Too few lines of Monkey definition")).unwrap()[1].parse().unwrap();
        let jump_true: usize = regex_true.captures(it.next().expect("Too few lines of Monkey definition")).unwrap()[1].parse().unwrap();
        let jump_false: usize = regex_false.captures(it.next().expect("Too few lines of Monkey definition")).unwrap()[1].parse().unwrap();

        let mut monkey = Monkey { op, m1, m2, policy: Jump { module: test, trueval: jump_true, falseval: jump_false }, inspected: 0, items: Vec::new() };
        for part in start_list.split(",") {
            monkey.appendItem(int::from_str(part.trim()).unwrap());
        }

        return (number, monkey);
    }
}