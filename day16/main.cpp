#include "lib/valve.h"
#include "lib/twosymbol.h"
#include "lib/tunnelmap.h"
#include "lib/instance.h"
#include "lib/instancewithelephant.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <list>
#include <memory>

using namespace std;

list<std::unique_ptr<Valve<TwoSymbol,int>>> valves;
Tunnelmap<Valve<TwoSymbol,int>> tunnelmap;

void read(string filename){
    ifstream file(filename,ios::in);
    istream& stream = file;
    
    std::regex move_regex("^Valve ([A-Z]{2}) has flow rate=(\\d+); tunnels? leads? to valves? ([A-Z]{2}(, [A-Z]{2})*)$", std::regex_constants::icase);
    std::smatch match;
    std::string line;
    while ( getline (stream,line) ) {
        if (std::regex_search(line, match, move_regex) && match.size()>=4) {
            TwoSymbol name;
            name[0]=((string)match[1]).at(0);
            name[1]=((string)match[1]).at(1);
            valves.push_back(std::make_unique<Valve<TwoSymbol,int>>(name,stoi(match[2])));
        }
    }
    
    // read a second time
    stream.clear();
    stream.seekg(0);
    while ( getline (stream,line) ) {
        if (std::regex_search(line, match, move_regex) && match.size()>=4) {
            TwoSymbol name,to;
            name[0]=((string)match[1]).at(0);
            name[1]=((string)match[1]).at(1);
            to[0]=((string)match[3]).at(0);
            to[1]=((string)match[3]).at(1);
            auto src=std::find_if(valves.begin(),valves.end(),[&name](auto& ptr){return ptr->getSignature()==name;});
            auto dest=std::find_if(valves.begin(),valves.end(),[&to](auto& ptr){return ptr->getSignature()==to;});
            tunnelmap.addConnection(**src,**dest);
            if(match.size()==5) 
              for(int i=2;i+4<=match[3].length();i+=4) {
                to[0]=((string)match[3]).at(i+2);
                to[1]=((string)match[3]).at(i+3);
                dest=std::find_if(valves.begin(),valves.end(),[&to](auto& ptr){return ptr->getSignature()==to;});
                tunnelmap.addConnection(**src,**dest);
              }
        }
    }
    file.close();
    
    auto vvv=&valves;
    std::cout << "CONNECTIONS: " << std::endl;
    for(const auto& elem : valves)
    {
       std::cout << elem->getDescription() << std::endl;
       for(const auto& con: tunnelmap.getConnection(*elem))
           std::cout << "  " << con.get().getDescription() << std::endl;
    }
    std::cout << std::endl;
}

std::reference_wrapper<Valve<TwoSymbol,int>> findValve(TwoSymbol name) {
    auto src=std::find_if(valves.begin(),valves.end(),[&name](auto& ptr){return ptr->getSignature()==name;});
    return std::ref(**src);
}

void puzzle1(){
    cout << "PART 1:" << endl;
    const int total_time=30;
    const TwoSymbol start{'A','A'};
    std::list<std::reference_wrapper<const Valve<TwoSymbol,int>>> empty;
    std::list<std::reference_wrapper<const Valve<TwoSymbol,int>>> allValvesWithPressure;
    for(auto& valve : valves)
        if(valve->getRate()>0)
            allValvesWithPressure.push_back(std::cref(*valve));
    auto problem = std::make_unique<Instance<int,Valve<TwoSymbol,int>>>(total_time,0,findValve(start).get(),true,tunnelmap,allValvesWithPressure,empty,Instance<int,Valve<TwoSymbol,int>>::start);
    int best=problem->branch(0);
    cout << problem->report() << endl;
    cout << "Most released pressure: " << best << endl;
}

void puzzle2(){
    cout << endl << "PART 2:" << endl;
    const int total_time=26;
    const TwoSymbol start{'A','A'};
    std::list<std::reference_wrapper<const Valve<TwoSymbol,int>>> empty1;
    std::list<std::reference_wrapper<const Valve<TwoSymbol,int>>> empty2;
    std::list<std::reference_wrapper<const Valve<TwoSymbol,int>>> allValvesWithPressure;
    for(auto& valve : valves)
        if(valve->getRate()>0)
            allValvesWithPressure.push_back(std::cref(*valve));
    auto problem = std::make_unique<InstanceWithElephant<int,Valve<TwoSymbol,int>>>(total_time,0,findValve(start).get(),true,findValve(start).get(),true,tunnelmap,allValvesWithPressure,empty1,empty2,Instance<int,Valve<TwoSymbol,int>>::start,Instance<int,Valve<TwoSymbol,int>>::start);
    int best=problem->branch(0);
    cout << problem->report() << endl;
    cout << "Most released pressure: " << best << endl;
}

int main(int argc, char *argv[]) {
    if(argc!=2) {
        cout<<"Provide input file as runtime argument";
        return 255;
    }
    
    read(argv[1]);
    puzzle1();
    puzzle2();
}


