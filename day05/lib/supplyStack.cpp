#include "supplyStack.h"
#include <regex>
#include <algorithm>

adventofcode::SupplyStack::SupplyStack() {};
adventofcode::SupplyStack::~SupplyStack() {};
    
std::istream& adventofcode::operator>> (std::istream& in, adventofcode::SupplyStack& ss) {
    std::list<std::string> input;
    std::string line;
    while (getline(in,line)) {
        if(line=="")
            break;
        input.push_back(line);
    }
    
    std::regex regex_key("(\\d+)");
    std::smatch match;
    auto it=input.rbegin();
    line=*it;
    for (size_t i = 0; i < line.size(); i += 4){
        std::string part=line.substr(i,4);
        if(!std::regex_search(part,match,regex_key))
            throw std::runtime_error("Cannot parse Stack number");
        int key=stoi(match[1].str());
        ss.stack_map[key];
    }
    
    std::regex regex("\\[(\\w)\\]");
    for(it++;it!=input.rend();it++) {
        auto map_it=ss.stack_map.begin();
        line=*it;
        for (size_t i = 0; i < line.size(); i += 4,map_it++){
            std::string part=line.substr(i,4);
            if(std::regex_search(part,match,regex))
                map_it->second.push_back(match[1].str()[0]);
        }
    }
    return in;
}

std::ostream& adventofcode::operator<<(std::ostream& out, const adventofcode::SupplyStack& ss) {
    std::for_each(ss.stack_map.begin(),ss.stack_map.end(),[](auto l){std::cout << l.second.back();});
    return out;
}

int  adventofcode::SupplyStack::performSingleMove(const int from_key,const int to_key,const uint number) {
    if(!stack_map.contains(from_key) || !stack_map.contains(to_key))
        return 0;
    auto from=&stack_map.at(from_key);
    auto to=&stack_map.at(to_key);
    for(int i=0;i<number;i++) {
        if(from->empty())
            return i;
        to->push_back(from->back());
        from->pop_back();
    }
    return number;
}

int  adventofcode::SupplyStack::performGroupedMove(const int from_key,const int to_key,const uint number) {
    if(!stack_map.contains(from_key) || !stack_map.contains(to_key))
        return 0;
    auto from=&stack_map.at(from_key);
    auto to=&stack_map.at(to_key);
    uint groupsize=std::min(number,(uint)from->size());
    auto it_start=from->end();
    advance(it_start,-(int)groupsize);
    std::copy(it_start,from->end(),std::back_inserter(*to));
    from->erase(it_start,from->end());
    return groupsize;
}

char& adventofcode::SupplyStack::operator[](int idx) {
    if(!stack_map.contains(idx))
        throw std::runtime_error("Stack does not exist");
    return stack_map.at(idx).back();
}
