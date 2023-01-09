#[derive(Copy, Clone, Debug, PartialEq)]
pub enum Direction { West, East, North, South }

impl Direction {
    pub fn next(&self) -> Direction
    {
        match *self {
            Direction::West => Direction::East,
            Direction::East => Direction::North,
            Direction::North => Direction::South,
            Direction::South => Direction::West,
        }
    }
}

#[derive(Default)]
pub struct Elf {
    pub x_pos: i32,
    pub y_pos: i32,
    suggested_move: Option<Direction>,
}

impl Elf {
    pub fn new(x: i32, y: i32) -> Elf {
        Elf { x_pos: x, y_pos: y, ..Default::default() }
    }
    fn doMove(&mut self) {
        match self.suggested_move {
            Some(dir) =>
                match dir {
                    Direction::West => self.x_pos -= 1,
                    Direction::East => self.x_pos += 1,
                    Direction::North => self.y_pos -= 1,
                    Direction::South => self.y_pos += 1,
                },
            _ => ()
        }
    }

    pub fn planMove(&mut self, policy: &mut MapPolicy) -> bool {
        let necessary = policy.isMoveNecessary(self);
        self.suggested_move = match necessary {true => policy.suggestedMove(self), false => None};
        if !necessary {
            return  false;
        }
        let (x, y) = match self.suggested_move {
            Some(dir) =>
                match dir {
                    Direction::West => (self.x_pos - 1, self.y_pos),
                    Direction::East => (self.x_pos + 1, self.y_pos),
                    Direction::North => (self.x_pos, self.y_pos - 1),
                    Direction::South => (self.x_pos, self.y_pos + 1)
                },
            None => (self.x_pos, self.y_pos)
        };
        if self.suggested_move != None {
            policy.map.as_mut().expect("Cavemap not set").markMove(x, y);
        }
        return true;
    }

    pub fn moveIfPossibleAndNecessary(&mut self, policy: &MapPolicy) {
        let (x, y) = match self.suggested_move {
            Some(dir) =>
                match dir {
                    Direction::West => (self.x_pos - 1, self.y_pos),
                    Direction::East => (self.x_pos + 1, self.y_pos),
                    Direction::North => (self.x_pos, self.y_pos - 1),
                    Direction::South => (self.x_pos, self.y_pos + 1)
                },
            None => (self.x_pos, self.y_pos)
        };

        if policy.canMove(x, y) {
            self.doMove();
        }
    }
}

#[derive(Default)]
pub struct CaveMap {
    nw: Vec<Vec<i8>>,
    sw: Vec<Vec<i8>>,
    ne: Vec<Vec<i8>>,
    se: Vec<Vec<i8>>,
}

impl CaveMap {
    pub fn new(x_min: i32, x_max: i32, y_min: i32, y_max: i32) -> CaveMap {
        let mut map = CaveMap::default();
        if x_min <= 0 && y_min <= 0 {
            map.nw = vec![vec![0; (-x_min+1) as usize]; (-y_min+1) as usize];
        }
        if x_min <= 0 && y_max >= -1 {
            map.sw = vec![vec![0; (-x_min+1) as usize]; (y_max + 2) as usize];
        }
        if x_max >= -1 && y_min <= 0 {
            map.ne = vec![vec![0; (x_max + 2) as usize]; (-y_min+1) as usize];
        }
        if x_max >= -1 && y_max >= -1 {
            map.se = vec![vec![0; (x_max + 2) as usize]; (y_max + 2) as usize];
        }
        return map;
    }

    pub fn newByElves(elves: &[Elf]) -> CaveMap {
        let minx = elves.iter().map(|p| p.x_pos).min().unwrap_or(0);
        let maxx = elves.iter().map(|p| p.x_pos).max().unwrap_or(0);
        let miny = elves.iter().map(|p| p.y_pos).min().unwrap_or(0);
        let maxy = elves.iter().map(|p| p.y_pos).max().unwrap_or(0);
        CaveMap::new(minx, maxx, miny, maxy)
    }

    pub fn mapElves(&mut self, elves: &[Elf]) {
        for elf in elves {
            match (elf.x_pos, elf.y_pos) {
                (x, y) if x < 0 && y < 0 => self.nw[-y as usize - 1][-x as usize - 1] = -1,
                (x, y) if x < 0 &&
                    y >= 0 => self.sw[y as usize][-x as usize - 1] = -1,
                (x, y) if x >= 0 &&
                    y < 0 => self.ne[-y as usize - 1][x as usize] = -1,
                (x, y) if x >= 0 &&
                    y >= 0 => self.se[y as usize][x as usize] = -1,
                (_, _) => panic!("impossible")
            }
        }
    }

    pub fn position(&self, x: i32, y: i32) -> i8 {
        match (x, y) {
            (x, y) if x < 0 && y < 0 => if (self.nw.len() > 0) { self.nw[-y as usize - 1][-x as usize - 1] } else { 0 },
            (x, y) if x < 0 && y >= 0 => if (self.sw.len() > 0) { self.sw[y as usize][-x as usize - 1] } else { 0 },
            (x, y) if x >= 0 && y < 0 => if (self.ne.len() > 0) { self.ne[-y as usize - 1][x as usize] } else { 0 },
            (x, y) if x >= 0 && y >= 0 => if (self.se.len() > 0) { self.se[y as usize][x as usize] } else { 0 },
            (_, _) => panic!("impossible")
        }
    }
    pub fn markMove(&mut self, x: i32, y: i32) {
        match (x, y) {
            (x, y) if x < 0 &&
                y < 0 => if (self.nw.len() > 0) {
                self.nw[-y as usize - 1][-x as usize - 1] += 1
            },
            (x, y) if x < 0 &&
                y >= 0 => if (self.sw.len() > 0) {
                self.sw[y as usize][-x as usize - 1] += 1
            },
            (x, y) if x >= 0 &&
                y < 0 => if (self.ne.len() > 0) {
                self.ne[-y as usize - 1][x as usize] += 1
            },
            (x, y) if x >= 0 &&
                y >= 0 => if (self.se.len() > 0) {
                self.se[y as usize][x as usize] += 1
            },
            (_, _) => panic!("impossible")
        }
    }

    pub fn print(&self) {
        for line in (0..self.ne.len()).rev() {
            if (self.nw.len() > 0) {
                self.nw[line].iter().rev().for_each(|n| match *n {
                    -1 => print!("{}", '#'),
                    0 => print!("{}", '.'),
                    x if x > 0 => print!("{x}"),
                    _ => panic!("unexpected marker")
                });
            }
            self.ne[line].iter().for_each(|n| match *n {
                -1 => print!("{}", '#'),
                0 => print!("{}", '.'),
                x if x > 0 => print!("{x}"),
                _ => panic!("unexpected marker")
            });
            println!();
        }
        for line in (0..self.se.len()) {
            if (self.sw.len() > 0) {
                self.sw[line].iter().rev().for_each(|n| match *n {
                    -1 => print!("{}", '#'),
                    0 => print!("{}", '.'),
                    x if x > 0 => print!("{x}"),
                    _ => panic!("unexpected marker")
                });
            }
            self.se[line].iter().for_each(|n| match *n {
                -1 => print!("{}", '#'),
                0 => print!("{}", '.'),
                x if x > 0 => print!("{x}"),
                _ => panic!("unexpected marker")
            });
            println!();
        }
        println!();
    }
}

pub struct MapPolicy {
    round: u16,
    map: Option<Box<CaveMap>>,
}

impl MapPolicy {
    pub fn nextRound(&mut self) {
        self.round += 1;
    }
    pub fn new() -> MapPolicy {
        MapPolicy { round: 1, map: None }
    }
    pub fn setMap(&mut self, m: Box<CaveMap>) {
        self.map = Some(m);
    }

    fn testDirection(&self, elf: &Elf, dir: Direction) -> bool {
        match dir {
            Direction::North => self.map.as_ref().expect("Cavemap not set").position(elf.x_pos - 1, elf.y_pos - 1) >= 0 && self.map.as_ref().expect("Cavemap not set").position(elf.x_pos, elf.y_pos - 1) >= 0 && self.map.as_ref().expect("Cavemap not set").position(elf.x_pos + 1, elf.y_pos - 1) >= 0,
            Direction::South => self.map.as_ref().expect("Cavemap not set").position(elf.x_pos - 1, elf.y_pos + 1) >= 0 && self.map.as_ref().expect("Cavemap not set").position(elf.x_pos, elf.y_pos + 1) >= 0 && self.map.as_ref().expect("Cavemap not set").position(elf.x_pos + 1, elf.y_pos + 1) >= 0,
            Direction::West => self.map.as_ref().expect("Cavemap not set").position(elf.x_pos - 1, elf.y_pos - 1) >= 0 && self.map.as_ref().expect("Cavemap not set").position(elf.x_pos - 1, elf.y_pos) >= 0 && self.map.as_ref().expect("Cavemap not set").position(elf.x_pos - 1, elf.y_pos + 1) >= 0,
            Direction::East => self.map.as_ref().expect("Cavemap not set").position(elf.x_pos + 1, elf.y_pos - 1) >= 0 && self.map.as_ref().expect("Cavemap not set").position(elf.x_pos + 1, elf.y_pos) >= 0 && self.map.as_ref().expect("Cavemap not set").position(elf.x_pos + 1, elf.y_pos + 1) >= 0,
        }
    }

    pub fn isMoveNecessary(&self, elf: &Elf) -> bool {
        !(self.testDirection(elf, Direction::South) &&self.testDirection(elf, Direction::North) &&self.testDirection(elf, Direction::West) &&self.testDirection(elf, Direction::East) )
    }

    pub fn suggestedMove(&self, elf: &Elf) -> Option<Direction> {
        let dir = match (self.round % 4) {
            1 => Direction::North,
            2 => Direction::South,
            3 => Direction::West,
            0 => Direction::East,
            4_u16..=u16::MAX => panic!("impossible")
        };
        if self.testDirection(elf, dir)
        { return Some(dir); }

        let dir = dir.next();
        if self.testDirection(elf, dir)
        { return Some(dir); }

        let dir = dir.next();
        if self.testDirection(elf, dir)
        { return Some(dir); }

        let dir = dir.next();
        if self.testDirection(elf, dir)
        { return Some(dir); }

        return Option::None;
    }

    pub fn canMove(&self, x: i32, y: i32) -> bool {
        self.map.as_ref().expect("Cavemap not set").position(x, y) == 1
    }

    pub fn printMap(&self) {
        self.map.as_ref().expect("Cavemap not set").print();
    }
}




