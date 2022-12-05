#ifndef SupplyStack_H
#define SupplyStack_H

#include <iostream>
#include <map>
#include <list>

namespace adventofcode {

class SupplyStack {
    private:
        std::map<int,std::list<char>> stack_map;
    
    public:
        SupplyStack();
        ~SupplyStack();
    
        // read until an empty line occurs and build the SupplyStack
        friend std::istream& operator>> (std::istream& in, SupplyStack& ss);
        // perform a number of moves one by one
        int performSingleMove(const int,const int,const uint);
        // perform a big move of a bunch 
        int performGroupedMove(const int,const int,const uint);
        // read top
        char& operator[](int idx);
        // output the top element of each stack
        friend std::ostream& operator<<(std::ostream& out, const SupplyStack& ss);
};

std::istream& operator>> (std::istream& in, SupplyStack& ss);
std::ostream& operator<<(std::ostream& out, const SupplyStack& ss);
}

#endif
