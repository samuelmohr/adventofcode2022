#include "lib/supplyStack.h"
#include <iostream>
#include <fstream>
#include <regex>

using namespace std;
using namespace adventofcode;


void puzzle(int task,string filename){
    ifstream file(filename,ios::in);
    istream& stream = file;
    
    SupplyStack stack;
    stream>>stack;
    
    std::regex move_regex("^move (\\d+) from (\\d+) to (\\d+)$", std::regex_constants::icase);
    std::smatch match;
    std::string line;
    while ( getline (stream,line) ) {
        if (std::regex_search(line, match, move_regex) && match.size()==4) {
            switch(task) {
                case 1:
                    stack.performSingleMove(stoi(match[2]),stoi(match[3]),stoi(match[1]));
                    break;
                case 2: 
                    stack.performGroupedMove(stoi(match[2]),stoi(match[3]),stoi(match[1]));
                    break;
                default:
                    return;
            }
        }
    }
    file.close();
    cout << "Simulation of CrateMover 900"<<(task-1)<<":" << endl;
    cout << stack << endl;    
}


int main(int argc, char *argv[]) {
    if(argc!=2) {
        cout<<"Provide input file as runtime argument";
        return 255;
    }
    
    puzzle(1,argv[1]);
    puzzle(2,argv[1]);
}
