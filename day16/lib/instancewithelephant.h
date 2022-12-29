#ifndef INSTANCEWITHELEPHANT_H
#define INSTANCEWITHELEPHANT_H

#include "tunnelmap.h"
#include "instance.h"
#include <list>
#include <string>
#include <memory>

// T is pressure, V is valve
template <class T,class V>
class InstanceWithElephant {

  private:
    const int total_time;
    const int current_time;
    const std::reference_wrapper<const V> valve[2];
    const bool possible_to_open[2];
    const std::reference_wrapper<const Tunnelmap<V>> tunnelmap;
     std::list<std::reference_wrapper<const V>> closed_valves;
    const std::list<std::reference_wrapper<const V>> untouched_passed_valves[2];
    std::unique_ptr<InstanceWithElephant<T,V>> next_instance;
    Instance<T,V>::Type type[2];

  public:
    InstanceWithElephant() = delete;
    InstanceWithElephant(Instance<T,V> &inst1,Instance<T,V> &inst2);
    InstanceWithElephant(const int total_time, const int current_time, const V &valve1, const bool possible_to_open1, const V &valve2, const bool possible_to_open2, const Tunnelmap<V> &tunnelmap, std::list<std::reference_wrapper<const V>> closed_valves, std::list<std::reference_wrapper<const V>> untouched_passed_valves1, std::list<std::reference_wrapper<const V>> untouched_passed_valves2, Instance<T,V>::Type type1, Instance<T,V>::Type type2);
    ~InstanceWithElephant() {}


    T branch(T to_exceed);
    T lowerbound() const;
    T upperbound() const;
    std::string report() const;

 protected:
    T lowerbound(int actor) const;
};

#endif // INSTANCEWITHELEPHANT_H
