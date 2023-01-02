mod monkey;

use crate::monkey::{Monkey};

use std::env;
use std::fs;
use std::cell::RefCell;
use std::fmt::Display;
use std::fmt::Debug;
use std::str::FromStr;
use num::Integer;

fn main() {
    let mut args=env::args();
    args.next();

    let file_path = match args.next() {
        Some(arg) => arg,
        None => panic!("Use program with input file as runtime argument."),
    };

    part1(&file_path);
    println!("\n\nPART 2\n");
    part2(&file_path);
}

fn part1(file_path: &String) {
    let worry_reducer = 3;
    let contents = fs::read_to_string(file_path)
        .expect("Should have been able to read the file");

    let mut monkeylist: Vec<RefCell<Monkey<i32>>> = Vec::new();
    for m_str in contents.split("\n\n") {
        let (number, monkey) = Monkey::new(m_str);
        if number != monkeylist.len().try_into().unwrap() {
            panic!("Bad monkey numbering");
        }
        monkeylist.push(RefCell::new(monkey));
    }

    for i in 0..20 {
        println!("ROUND {i}");
        for (number, monkey) in monkeylist.iter().enumerate() { monkey.borrow().printMonkey(number.try_into().unwrap()); }
        simulateRound(&mut monkeylist, worry_reducer,Option::None);
    }

    println!("AFTER 20 ROUND");
    for (number, monkey) in monkeylist.iter().enumerate() { monkey.borrow().printMonkey(number.try_into().unwrap()); }

    println!();
    println!("Monkey Business Coefficient: {}", monkeyBusiness(&monkeylist));
}

fn part2(file_path: &String) {
    let worry_reducer = 1;
    let contents = fs::read_to_string(file_path)
        .expect("Should have been able to read the file");

    let mut monkeylist: Vec<RefCell<Monkey<i64>>> = Vec::new();
    for m_str in contents.split("\n\n") {
        let (number, monkey) = Monkey::new(m_str);
        if number != monkeylist.len().try_into().unwrap() {
            panic!("Bad monkey numbering");
        }
        monkeylist.push(RefCell::new(monkey));
    }

    let modulo_cap = monkeylist.iter().fold(1,|val,add| val*<i16 as Into<i64>>::into(add.borrow().getDecisionDivisor()));

    for i in 0..10 {
        let rounds=i*1000;
        println!("ROUND {rounds}");
        for (number, monkey) in monkeylist.iter().enumerate() { monkey.borrow().printInspectedCount(number.try_into().unwrap()); }
        for _ in 0..1000 {  simulateRound(&mut monkeylist, worry_reducer, Option::Some(modulo_cap)); }
    }

    println!("AFTER 10000 ROUND");
    for (number, monkey) in monkeylist.iter().enumerate() { monkey.borrow().printInspectedCount(number.try_into().unwrap()); }

    println!();
    println!("Monkey Business Coefficient: {}", monkeyBusiness(&monkeylist));
}


fn monkeyBusiness<int>(monkeylist: &[RefCell<Monkey<int>>]) -> u64
    where int: Integer + Display + From<i16> + FromStr + Copy, <int as FromStr>::Err:Debug
{
    let mut inspected: Vec<u32> = monkeylist.iter().map(|monkey| monkey.borrow().getInspectedCount()).collect();
    inspected.sort();
    return <u32 as Into<u64>>::into(inspected[inspected.len() - 1]) * <u32 as Into<u64>>::into(inspected[inspected.len() - 2]);
}

fn simulateRound<int>(monkeylist: &mut Vec<RefCell<Monkey<int>>>, worry_reducer: i16, modulo_cap: Option<int>)
    where int: Integer + Display + From<i16> + FromStr + Copy, <int as FromStr>::Err:Debug
{
    for monkey in &*monkeylist {
        monkey.borrow_mut().inspectAll(monkeylist, worry_reducer,modulo_cap);
    }
}