use std::env;
use std::fs;
use regex::Regex;
use crate::Direction::top;

fn main() {
    let mut args = env::args();
    args.next();

    let file_path = match args.next() {
        Some(arg) => arg,
        None => panic!("Use program with input file as runtime argument."),
    };

    part(&file_path,do_step_rule1);
    println!("\n\nPART 2\n");
    part(&file_path, do_step_rule2);
}

type step_function = fn (&mut (usize, usize),  &mut Direction, &Vec<&str>);

#[derive(Copy, Clone, Debug, PartialEq)]
enum Direction { right, top, left, bottom }

impl Direction {
    fn turn(self, char: char) -> Direction {
        match char {
            'R' => match self {
                Direction::right => Direction::bottom,
                Direction::bottom => Direction::left,
                Direction::left => Direction::top,
                Direction::top => Direction::right
            },
            'L' => match self {
                Direction::right => Direction::top,
                Direction::bottom => Direction::right,
                Direction::left => Direction::bottom,
                Direction::top => Direction::left
            },
            _ => panic!("Rotations can only be R and L")
        }
    }
    fn unwrap(self) -> i32 {
        match self { Direction::right => 0, Direction::bottom => 1, Direction::left => 2, Direction::top => 3}
    }
}

fn part(file_path: &String, do_step : step_function) {
    let contents = fs::read_to_string(file_path)
        .expect("Should have been able to read the file");

    let mut parts = contents.split("\n\n");
    let area: Vec<&str> = parts.next().unwrap().lines().collect();

    let mut pos: (usize, usize) = (0, area[0].find(".").expect("No free field in first line"));
    let mut direction: Direction = Direction::right;

    let regex: Regex = Regex::new("(\\d+[R|L])").unwrap();

    let cmd_str = parts.next().expect("No commands given").trim();
    let commands = regex.find_iter(cmd_str);

    for command in commands {
        let rmatch = command.as_str();
        let steps: u16 = rmatch[..rmatch.len() - 1].parse().unwrap();
        let turn: char = rmatch.chars().last().unwrap();

        for _ in 0..steps { do_step(&mut pos, &mut direction, &area); }
        do_turn(&mut direction, turn);
    }

    let last_steps: u16 = Regex::new("(\\d+)$").unwrap().find(cmd_str).unwrap().as_str().parse().unwrap();
    for _ in 0..last_steps { do_step(&mut pos, &mut direction, &area); }
    println!("The last position is ({:?}) facing direction {:?}", pos, direction);
    println!("The passwort is {}",password(&pos,&direction));
}

fn do_step_rule1((x, y): &mut (usize, usize), direction: &mut Direction, area: &Vec<&str>) {
    match direction {
        Direction::right =>
            if (*y < area[*x].len() - 1) {
                if area[*x].chars().nth(*y + 1).unwrap() != '#' {
                    *y += 1;
                }
            } else {
                if area[*x].trim().get(0..1).unwrap() != "#" {
                    *y = area[*x].find('.').unwrap();
                }
            },
        Direction::left => {
            if (*y > 0) {
                if area[*x].chars().nth(*y - 1).unwrap() == '.' {
                    *y -= 1;
                    return;
                } else if area[*x].chars().nth(*y - 1).unwrap() == '#' {
                    return;
                }
            }
            if area[*x].chars().last().unwrap() == '.' {
                *y = area[*x].len() - 1;
            }
        }
        Direction::bottom => {
            if (*x < area.len() - 1) {
                if area[*x + 1].chars().nth(*y).unwrap_or(' ') == '.' {
                    *x += 1;
                    return;
                } else if area[*x + 1].chars().nth(*y).unwrap_or(' ')== '#' {
                    return;
                }
            }
            for i in 0..area.len() {
                if area[i].chars().nth(*y).unwrap() == '#' {
                    return;
                } else if area[i].chars().nth(*y).unwrap() == '.' {
                    *x = i;
                    return;
                }
            }
            panic!("no free space in column");
        },
        Direction::top => {
            if (*x >0) {
                if area[*x - 1].chars().nth(*y).unwrap() == '.' {
                    *x -= 1;
                    return;
                } else if area[*x - 1].chars().nth(*y).unwrap() == '#' {
                    return;
                }
            }
            for i in (0..area.len()).rev() {
                if area[i].chars().nth(*y).unwrap_or(' ') == '#' {
                    return;
                } else if area[i].chars().nth(*y).unwrap_or(' ') == '.' {
                    *x = i;
                    return;
                }
            }
            panic!("no free space in column");
        }
    }
}

fn do_turn(direction: &mut Direction, char: char) {
    *direction = direction.turn(char);
}

fn password((x, y): &(usize, usize), direction: &Direction) -> i32 {
    (*x as i32 +1)*1000+(*y as i32+1)*4+direction.unwrap()
}

fn do_step_rule2((x, y): &mut (usize, usize), direction: &mut Direction, area: &Vec<&str>) {
    if area.len()%4 != 0 { panic!("Unexpected cube layout"); }
    let cube_size=area.len()/4;

    let (xn,yn,dn) = getPosition(cube_size,&(*x,*y),direction);
    match area[xn].chars().nth(yn).unwrap() {
        '#' => {return;}
        '.' => {
            *x=xn;
            *y=yn;
            *direction=dn;
        },
        _ => {panic!("moved out of bound");}
    }

}

fn getPosition(cube_size: usize, (x,y) :&(usize,usize), direction: &Direction) -> (usize,usize,Direction) {
    //  ##
    //  #
    // ##
    // #
    let x_side=match *x {
        s if s<cube_size=> 0,
        s if s<2*cube_size=> 1,
        s if s<3*cube_size=> 2,
        s if s<4*cube_size=> 3,
        _ => panic!("out of bound")
    };
    let y_side=match *y {
        s if s<cube_size=> 0,
        s if s<2*cube_size=> 1,
        s if s<3*cube_size=> 2,
        _ => panic!("out of bound")
    };
    match (x_side,y_side) {
        (0, 1) => if *x == 0 && *direction == Direction::top { (*y + 2 * cube_size, 0, Direction::right) } else if *y == cube_size && *direction == Direction::left { (3 * cube_size - 1 - *x, 0, Direction::right) } else {
            (match direction {
                Direction::top => *x - 1,
                Direction::bottom => *x + 1,
                _ => *x
            }, match direction {
                Direction::right => *y + 1,
                Direction::left => *y - 1,
                _ => *y
            }, *direction)
        },
        (0, 2) => if *x == 0 && *direction == Direction::top { (4 * cube_size-1, *y-2*cube_size, Direction::top) } else if *y == 3*cube_size-1 && *direction == Direction::right { (3 * cube_size - 1 - *x, 2*cube_size-1, Direction::left) } else if *x == cube_size-1 && *direction == Direction::bottom { (*y-cube_size, 2*cube_size-1, Direction::left) } else {
            (match direction {
                Direction::top => *x - 1,
                Direction::bottom => *x + 1,
                _ => *x
            }, match direction {
                Direction::right => *y + 1,
                Direction::left => *y - 1,
                _ => *y
            }, *direction)
        },
        (1, 1) => if *y == cube_size && *direction == Direction::left { (2* cube_size, *x - cube_size, Direction::bottom) } else if *y == 2* cube_size-1 && *direction == Direction::right { (cube_size-1, cube_size + *x, Direction::top) } else {
            (match direction {
                Direction::top => *x - 1,
                Direction::bottom => *x + 1,
                _ => *x
            }, match direction {
                Direction::right => *y + 1,
                Direction::left => *y - 1,
                _ => *y
            }, *direction)
        },
        (2, 1) => if *x == 3*cube_size-1 && *direction == Direction::bottom { (2* cube_size+*y,cube_size-1, Direction::left) } else if *y == 2* cube_size-1 && *direction == Direction::right { (3*cube_size-1-*x, 3*cube_size -1, Direction::left) } else {
            (match direction {
                Direction::top => *x - 1,
                Direction::bottom => *x + 1,
                _ => *x
            }, match direction {
                Direction::right => *y + 1,
                Direction::left => *y - 1,
                _ => *y
            }, *direction)
        },
        (2, 0) => if *x == 2*cube_size && *direction == Direction::top { (*y + cube_size, cube_size, Direction::right) } else if *y == 0 && *direction == Direction::left { (3 * cube_size - 1 - *x, cube_size, Direction::right) } else {
            (match direction {
                Direction::top => *x - 1,
                Direction::bottom => *x + 1,
                _ => *x
            }, match direction {
                Direction::right => *y + 1,
                Direction::left => *y - 1,
                _ => *y
            }, *direction)
        },
        (3, 0) => if *x == 4*cube_size-1 && *direction == Direction::bottom { (0, 2*cube_size+*y, Direction::bottom) } else if *y == cube_size-1 && *direction == Direction::right { (3*cube_size-1, *x-2 * cube_size,  Direction::top) } else if *y == 0 && *direction == Direction::left { (0, *x-2*cube_size, Direction::bottom) } else {
            (match direction {
                Direction::top => *x - 1,
                Direction::bottom => *x + 1,
                _ => *x
            }, match direction {
                Direction::right => *y + 1,
                Direction::left => *y - 1,
                _ => *y
            }, *direction)
        },
        _ => panic!("out of field")
    }

}