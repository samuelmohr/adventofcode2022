#ifndef INSTANCE_H
#define INSTANCE_H

#include "tunnelmap.h"
#include <list>
#include <string>
#include <memory>

// T is pressure, V is valve
//template <class T,class V>
//class InstanceWithElephants;

template <class T,class V>
class Instance {
  public:
      enum Type { undecided, start, pass, open, end};

  public:
    const int total_time;
    const int current_time;
    const std::reference_wrapper<const V> valve;
    const bool possible_to_open;
    const std::reference_wrapper<const Tunnelmap<V>> tunnelmap;
    const std::list<std::reference_wrapper<const V>> closed_valves;
    const std::list<std::reference_wrapper<const V>> untouched_passed_valves;
    std::unique_ptr<Instance<T,V>> next_instance;
    Type type;

  public:
    Instance() = delete;
    Instance(const int total_time, const int current_time, const V &valve, const bool possible_to_open, const Tunnelmap<V> &tunnelmap, std::list<std::reference_wrapper<const V>> closed_valves, std::list<std::reference_wrapper<const V>> untouched_passed_valves, Type type);
    ~Instance() {}


    T branch(T to_exceed);
    T lowerbound() const;
    T upperbound() const;
    std::string report() const;

    friend bool operator==(const std::reference_wrapper<const V>& lhs, const std::reference_wrapper<const V>& rhs);
//    friend class InstanceWithElephants<T,V>;
};

#endif // INSTANCE_H
