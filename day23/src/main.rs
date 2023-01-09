mod landscape;

use std::env;
use std::fs;

use crate::landscape::Elf;
use crate::landscape::MapPolicy;
use crate::landscape::CaveMap;

fn main() {
    let mut args = env::args();
    args.next();

    let file_path = match args.next() {
        Some(arg) => arg,
        None => panic!("Use program with input file as runtime argument."),
    };

    simulate(&file_path);
}

fn squareSize(elves: &[Elf]) -> i32 {
    let minx = elves.iter().map(|p| p.x_pos).min().unwrap_or(0);
    let maxx = elves.iter().map(|p| p.x_pos).max().unwrap_or(0);
    let miny = elves.iter().map(|p| p.y_pos).min().unwrap_or(0);
    let maxy = elves.iter().map(|p| p.y_pos).max().unwrap_or(0);
    return (maxx - minx +1) * (maxy - miny+1);
}


fn simulate(file_path: &String) {
    let contents = fs::read_to_string(file_path)
        .expect("Should have been able to read the file");

    let mut elves = Vec::new();
    for (nr, line) in contents.lines().enumerate() {
        for (i, c) in line.chars().enumerate() {
            if c == '#' {
                elves.push(Elf::new(i as i32, nr as i32));
            }
        }
    }
    println!("Successfully created {} elves",elves.len());

    // test initial positions
    let mut map = CaveMap::newByElves(&elves);
    map.mapElves(&elves);
    println!("== Initial setting ==");
    map.print();

    // Define policy and run 10 rounds
    let mut mp: MapPolicy = MapPolicy::new();
    for r in 0..10 {
        let mut map = Box::new(CaveMap::newByElves(&elves));
        map.mapElves(&elves);
        mp.setMap(map);

        // first half
        for elf in &mut elves {
            elf.planMove(&mut mp);
        }

        println!("== Round {} planning ==", r + 1);
        mp.printMap();

        // second
        for elf in &mut elves {
            elf.moveIfPossibleAndNecessary(&mp);
        }
        mp.nextRound();
    }

    println!("== Final distribution ==");
    let mut map = CaveMap::newByElves(&elves);
    map.mapElves(&elves);
    map.print();


    let answer = squareSize(&elves) - elves.len() as i32;
    println!("The number of empty ground tiles is {answer}");
    println!("We are on a right track");

    println!("\n\nPART 2\n");
// finish simulation
    let mut hasElfMoved = true;
    let mut round = 10;
    while hasElfMoved {
        let mut map = Box::new(CaveMap::newByElves(&elves));
        map.mapElves(&elves);
        mp.setMap(map);

        hasElfMoved = false;

        // first half
        for elf in &mut elves {
            hasElfMoved |= elf.planMove(&mut mp);
        }

        // second
        for elf in &mut elves {
            elf.moveIfPossibleAndNecessary(&mp);
        }
        mp.nextRound();
        round +=1;
    }

    println!("== Final distribution ==");
    let mut map = CaveMap::newByElves(&elves);
    map.mapElves(&elves);
    map.print();

    println!("The number of then first round without moving elves is {}",round);
}


