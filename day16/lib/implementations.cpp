#include "valve.h"
#include "twosymbol.h"
#include "tunnelmap.h"
#include "instance.h"
#include "instancewithelephant.h"
#include <numeric>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <iostream>

// CLASS VALVE
template <class S, class T>
Valve<S,T>::Valve(const S &s) : signature(s),rate(0) {}
template <class S, class T>
Valve<S,T>::Valve(const S &s, const T r) : signature(s),rate(r) {}
template <class S, class T>
Valve<S,T>::~Valve() {}
template <class S, class T>
Valve<S,T>::Valve(const Valve<S,T> &old): signature(old.signature),rate(old.rate) {}

template <class S, class T>
T Valve<S,T>::getRate() const {return rate; }
template <class S, class T>
Valve<S,T>::operator T() const {return rate; }
template <class S, class T>
const bool Valve<S,T>::operator<(const Valve<S,T> &r) const {return rate<r.rate;}
template <class S, class T>
const bool Valve<S,T>::operator>(const Valve<S,T> &r) const {return rate>r.rate;}


template <class S, class T>
S Valve<S,T>::getSignature() const {return signature; }
template <class S, class T>
Valve<S,T>::operator std::string() const {return (std::string)getSignature(); }
template <class S, class T>
std::string Valve<S,T>::getDescription() const {return "Valve "+((std::string)getSignature()); }
template <class S, class T>
const bool Valve<S,T>::operator==(const Valve<S,T> &r) const {return signature==r.signature;}



// CLASS TWOSYMBOL
TwoSymbol::TwoSymbol(char c1, char c2) {symb[0]=c1; symb[1]=c2; }
char& TwoSymbol::operator [](int idx) {return symb[idx];}
char TwoSymbol::operator [](int idx) const {return symb[idx];}
bool operator==(const TwoSymbol& lhs, const TwoSymbol& rhs) {return lhs.symb[0]==rhs.symb[0] && lhs.symb[1]==rhs.symb[1];}
TwoSymbol::operator std::string() const {return std::string(symb,2);}


// CLASS TUNNELMAP
template<class V>
bool Tunnelmap<V>::addConnection(const V &from, const V &to) {
    auto it=connections.find(std::cref(from));
    if(it!=connections.end())
      it->second.push_back(std::cref(to));
    else
      connections.insert({std::cref(from),std::list(1,std::cref(to))});
    return true;
  }

template <class V>
std::list<std::reference_wrapper<const V>> Tunnelmap<V>::getConnection(const V &valve) const
{return connections.find(std::cref(valve))->second;}


// CLASS INSTANCE
template<class T, class V>
Instance<T, V>::Instance(const int tt, const int ct, const V &v, const bool pos, const Tunnelmap<V> &tm, std::list<std::reference_wrapper<const V> > closed, std::list<std::reference_wrapper<const V> > passed, Type t)
    : total_time(tt),current_time(ct),
      closed_valves(closed),untouched_passed_valves(passed),
      tunnelmap(std::cref(tm)),valve(std::cref(v)),
      type(t),possible_to_open(pos),
      next_instance(std::unique_ptr<Instance<T,V>>(nullptr)) { }

template<class T, class V>
T Instance<T, V>::lowerbound() const
{
    if(!possible_to_open || type==end || total_time==current_time)
        return 0;
    T val= (total_time-current_time-(type==open?0:1))*((T)valve.get());
    if(val<0)
        throw std::runtime_error("Bad implementation of lower bound");
    return val;
}

template<class T, class V>
T Instance<T, V>::upperbound() const
{
    T val=0;
    if(possible_to_open && type!=open && valve.get().getRate()>0) {
        // open first, then continue
        if(total_time-current_time<=1)
            val=0;
        else if(total_time-current_time<=3)
            val=(total_time-current_time-1)*((T)valve.get());
        else {
          std::list<T> copy;
          for(auto& v : closed_valves)
            if(v.get()!=valve.get())
              copy.push_back(v.get().getRate());
          copy.sort();
          copy.reverse();
          int i=0;
          for(auto it=copy.begin();it!=copy.end();i++,it++)
            *it=(*it)*(total_time-current_time-3-2*i);
          std::partial_sum(copy.begin(),copy.end(),copy.begin());
          auto it=copy.begin();
          std::ranges::advance(it,(total_time-current_time-4)/2,std::prev(copy.end()));
          val=(total_time-current_time-1)*((T)valve.get())+(*it);
        }
    }
    // walk to the highest, open, walk, open, …
    if(total_time-current_time>=3) {
      std::list<T> copy;
      for(auto& valve : closed_valves)
        copy.push_back(valve.get().getRate());
      copy.sort();
      copy.reverse();
      int i=0;
      for(auto it=copy.begin();it!=copy.end();i++,it++)
        *it=(*it)*(total_time-current_time-2-2*i);
      std::partial_sum(copy.begin(),copy.end(),copy.begin());
      auto it=copy.begin();
      std::ranges::advance(it,(total_time-current_time-3)/2,std::prev(copy.end()));
      val=std::max(val,*it);
    }
    return val+(type==open?(total_time-current_time)*((T)valve.get()):0);
}

template<class T, class V>
std::string Instance<T, V>::report() const
{
    std::string out="== Minute "+std::to_string(current_time)+" ==\n";
    switch(type) {
      case start:
        out+="  -> Starting from "+valve.get().getDescription()+"\n";
        break;
      case pass:
        out+="  -> Walking to "+valve.get().getDescription()+"\n";
        break;
      case end:
        out+="  -> No need to walk anywhere, just waiting. \n";
        break;
      case open:
        out+="  -> Open "+valve.get().getDescription()+" with pressure release of "+std::to_string(valve.get().getRate())+" from next round\n";
        break;
      case undecided:
        out+="  -> Beamed to "+valve.get().getDescription()+"\n";
        break;
    }
    if(type!=undecided && next_instance!=nullptr)
      out+=next_instance->report();
    return out;
}


template <class T,class V>
T Instance<T,V>::branch(T to_exceed) {
  if(current_time==total_time) {
      type=end;
      return 0;
  }

  T own_contribution=type==open?(total_time-current_time)*((T)valve.get()):0;

  std::list<std::unique_ptr<Instance<T,V>>> options;
  if(type!=open && possible_to_open && valve.get().getRate()>0) {
    auto updated_closed=closed_valves;
    auto ref=valve;
    updated_closed.remove_if([ref](std::reference_wrapper<const V> &v){return v.get()==ref.get();});
    std::list<std::reference_wrapper<const V>> empty_list;
    options.push_back(std::make_unique<Instance<T,V>>(total_time,current_time+1,valve.get(),true,tunnelmap.get(),updated_closed,empty_list,Type::open));
  }
  if(closed_valves.size()>0) {
    auto links=tunnelmap.get().getConnection(valve.get());
    auto update_untouched=untouched_passed_valves;
    update_untouched.push_back(std::cref(valve.get()));
    for(auto it=links.begin();it!=links.end();it++) {
      if(std::find(untouched_passed_valves.begin(),untouched_passed_valves.end(),*it)==untouched_passed_valves.end()) {
        bool can_open=(std::find(closed_valves.begin(),closed_valves.end(),*it)!=closed_valves.end());
        options.push_back(std::make_unique<Instance<T,V>>(total_time,current_time+1,it->get(),can_open,tunnelmap.get(),closed_valves,update_untouched,Type::pass));
      }
    }
  }

  T current_max=0;
  if(options.size()>0) {
    // sort list by maximum value, descending
    options.sort([](const std::unique_ptr<Instance<T,V>>& a, const std::unique_ptr<Instance<T,V>>& b) {return a->lowerbound()>=b->lowerbound();});
    current_max=options.front()->lowerbound();
    std::unique_ptr<Instance<T,V>> best(nullptr);
    for(auto it=options.begin();it!=options.end();it++) {
        T ub=(*it)->upperbound();
        if(ub<0)
            throw std::runtime_error("Bad implementation of upper bound");
      if(ub<to_exceed-own_contribution || current_max>ub) continue;
      T out=(*it)->branch(current_max);
      if(out>ub)
          throw std::runtime_error("Bad implementation of upper bound");
      if(out<(*it)->lowerbound())
          throw std::runtime_error("Bad implementation of upper bound");
      if(out>=current_max) {
        current_max=out;
        best=std::move(*it);
        if(best==nullptr)
            throw std::runtime_error("Undefined pointer");
      }
    }

    if(best!=nullptr)
        next_instance=std::move(best);
    else
        next_instance=std::make_unique<Instance<T,V>>(total_time,current_time+1,valve.get(),false,tunnelmap.get(),closed_valves,untouched_passed_valves,Type::end);
  } else {
      next_instance=std::make_unique<Instance<T,V>>(total_time,current_time+1,valve.get(),false,tunnelmap.get(),closed_valves,untouched_passed_valves,Type::end);
  }
  return current_max+own_contribution;
}

template<class V>
bool operator==(const std::reference_wrapper<const V>& lhs, const std::reference_wrapper<const V>& rhs) {return lhs.get()==rhs.get();}

// CLASS INSTANCEWITHELEPHANT
template<class T, class V>
InstanceWithElephant<T, V>::InstanceWithElephant(const int tt, const int ct, const V &v1, const bool pos1, const V &v2, const bool pos2, const Tunnelmap<V> &tm, std::list<std::reference_wrapper<const V> > closed, std::list<std::reference_wrapper<const V> > passed1, std::list<std::reference_wrapper<const V> > passed2, Instance<T,V>::Type t1, Instance<T,V>::Type t2)
    : total_time(tt),current_time(ct),
      closed_valves(closed),untouched_passed_valves(passed1,passed2),
      tunnelmap(std::cref(tm)),valve(std::cref(v1),std::cref(v2)),
      type(t1,t2),possible_to_open(pos1,pos2),
      next_instance(std::unique_ptr<InstanceWithElephant<T,V>>(nullptr)) { }

template<class T, class V>
InstanceWithElephant<T, V>::InstanceWithElephant(Instance<T, V> &inst1, Instance<T, V> &inst2)
    : total_time(inst1.total_time),current_time(inst1.current_time),
    untouched_passed_valves(inst1.untouched_passed_valves,inst2.untouched_passed_valves),
    tunnelmap(std::cref(inst1.tunnelmap.get())),valve(std::cref(inst1.valve.get()),std::cref(inst2.valve.get())),
    type(inst1.type,inst2.type),possible_to_open(inst1.possible_to_open,inst2.possible_to_open),
    next_instance(std::unique_ptr<InstanceWithElephant<T,V>>(nullptr))
{
    if(inst1.total_time!=inst2.total_time)
        throw std::runtime_error("Both problem instances have to be of the same type. ");
    if(inst1.current_time!=inst2.current_time)
        throw std::runtime_error("Both problem instances have to be of the same type. ");
    if(inst1.type!=Instance<T,V>::Type::open)
        closed_valves=inst2.closed_valves;
    else
        if(inst2.type!=Instance<T,V>::Type::open)
            closed_valves=inst1.closed_valves;
        else{
            auto l=inst1.closed_valves;
            for(auto it=l.begin();it!=l.end();) {
                if(std::find(inst2.closed_valves.begin(),inst2.closed_valves.end(),*it)==inst2.closed_valves.end())
                    l.erase(it++);
                else
                    it++;
            }
            closed_valves=l;
        }
}

template<class T, class V>
T InstanceWithElephant<T, V>::lowerbound() const
{
    if(valve[0].get()!=valve[1].get())
        return lowerbound(0)+lowerbound(1);
    return lowerbound(0);
}
template<class T, class V>
T InstanceWithElephant<T, V>::lowerbound(int actor) const
{
    if(!possible_to_open[actor] || type[actor]==Instance<T,V>::Type::end || total_time==current_time)
        return 0;
    T val= (total_time-current_time-(type[actor]==Instance<T,V>::Type::open?0:1))*((T)valve[actor].get());
    if(val<0)
        throw std::runtime_error("Bad implementation of lower bound");
    return val;
}

template<class T, class V>
T InstanceWithElephant<T, V>::upperbound() const
{
    // contribution from this case
    T own_contribution=0;
    if(type[0]==Instance<T,V>::Type::open){
        own_contribution+=(total_time-current_time)*((T)valve[0].get());
        if(type[1]==Instance<T,V>::Type::open && valve[0].get()!=valve[1].get())
            own_contribution+=(total_time-current_time)*((T)valve[1].get());
    }else if(type[1]==Instance<T,V>::Type::open)
        own_contribution+=(total_time-current_time)*((T)valve[1].get());

    // keep it super simple, just add the best of each case.
    T val=0;
    if(possible_to_open[0] && type[0]!=Instance<T,V>::Type::open && valve[0].get().getRate()>0) {
        // open first, then continue
        if(total_time-current_time<=1)
            val+=0;
        else if(total_time-current_time<=3)
            val+=(total_time-current_time-1)*((T)valve[0].get());
        else {
          std::list<T> copy;
          for(auto& v : closed_valves)
            if(v.get()!=valve[0].get())
              copy.push_back(v.get().getRate());
          copy.sort();
          copy.reverse();
          int i=0;
          for(auto it=copy.begin();it!=copy.end();i++,it++)
            *it=(*it)*(total_time-current_time-3-2*i);
          std::partial_sum(copy.begin(),copy.end(),copy.begin());
          auto it=copy.begin();
          std::ranges::advance(it,(total_time-current_time-4)/2,std::prev(copy.end()));
          val+=(total_time-current_time-1)*((T)valve[0].get())+(*it);
        }
    }
    if(possible_to_open[1] && type[1]!=Instance<T,V>::Type::open && valve[1].get().getRate()>0) {
        // open first, then continue
        if(total_time-current_time<=1)
            val+=0;
        else if(total_time-current_time<=3)
            val+=(total_time-current_time-1)*((T)valve[1].get());
        else {
          std::list<T> copy;
          for(auto& v : closed_valves)
            if(v.get()!=valve[1].get())
              copy.push_back(v.get().getRate());
          copy.sort();
          copy.reverse();
          int i=0;
          for(auto it=copy.begin();it!=copy.end();i++,it++)
            *it=(*it)*(total_time-current_time-3-2*i);
          std::partial_sum(copy.begin(),copy.end(),copy.begin());
          auto it=copy.begin();
          std::ranges::advance(it,(total_time-current_time-4)/2,std::prev(copy.end()));
          val+=(total_time-current_time-1)*((T)valve[1].get())+(*it);
        }
    }
    // walk to the highest, open, walk, open, …
    if(total_time-current_time>=3) {
      std::list<T> copy;
      for(auto& valve : closed_valves)
        copy.push_back(valve.get().getRate());
      copy.sort();
      copy.reverse();
      int i=0;
      for(auto it=copy.begin();it!=copy.end();i++,it++)
        *it=(*it)*(total_time-current_time-2-2*(i/2));
      std::partial_sum(copy.begin(),copy.end(),copy.begin());
      auto it=copy.begin();
      std::ranges::advance(it,((total_time-current_time-3)/2)*2+1,std::prev(copy.end()));
      val+=(*it);
    }
    return val+own_contribution;
}

template<class T, class V>
std::string InstanceWithElephant<T, V>::report() const
{
    std::string out="== Minute "+std::to_string(current_time)+" ==\n";
    switch(type[0]) {
      case Instance<T,V>::Type::start:
        out+="  -> Starting from "+valve[0].get().getDescription()+"\n";
        break;
      case Instance<T,V>::Type::pass:
        out+="  -> Walking to "+valve[0].get().getDescription()+"\n";
        break;
      case Instance<T,V>::Type::end:
        out+="  -> No need to walk anywhere, just waiting. \n";
        break;
      case Instance<T,V>::Type::open:
        out+="  -> Open "+valve[0].get().getDescription()+" with pressure release of "+std::to_string(valve[0].get().getRate())+" from next round\n";
        break;
      case Instance<T,V>::Type::undecided:
        out+="  -> Beamed to "+valve[0].get().getDescription()+"\n";
        break;
    }
    switch(type[1]) {
      case Instance<T,V>::Type::start:
        out+="  -> Elephant starts from "+valve[1].get().getDescription()+"\n";
        break;
      case Instance<T,V>::Type::pass:
        out+="  -> Elephant walks to "+valve[1].get().getDescription()+"\n";
        break;
      case Instance<T,V>::Type::end:
        out+="  -> No need for the elephant to walk anywhere, just waiting. \n";
        break;
      case Instance<T,V>::Type::open:
        out+="  -> Elephant opens "+valve[1].get().getDescription()+" with pressure release of "+std::to_string(valve[1].get().getRate())+" from next round\n";
        break;
      case Instance<T,V>::Type::undecided:
        out+="  -> Elephant beamed to "+valve[1].get().getDescription()+"\n";
        break;
    }
    if(next_instance!=nullptr)
      out+=next_instance->report();
    return out;
}


template <class T,class V>
T InstanceWithElephant<T,V>::branch(T to_exceed) {
  if(current_time==total_time) {
      type[0]=Instance<T,V>::Type::end;
      type[1]=Instance<T,V>::Type::end;
      return 0;
  }

  T own_contribution=0;
  if(type[0]==Instance<T,V>::Type::open){
      own_contribution+=(total_time-current_time)*((T)valve[0].get());
      if(type[1]==Instance<T,V>::Type::open && valve[0].get()!=valve[1].get())
          own_contribution+=(total_time-current_time)*((T)valve[1].get());
  }else if(type[1]==Instance<T,V>::Type::open)
      own_contribution+=(total_time-current_time)*((T)valve[1].get());

  std::list<std::unique_ptr<Instance<T,V>>> options1;
  if(type[0]!=Instance<T,V>::Type::open && possible_to_open[0] && valve[0].get().getRate()>0 && (std::find(closed_valves.begin(),closed_valves.end(),valve[0].get())!=closed_valves.end())) {
    auto updated_closed=closed_valves;
    auto ref=valve[0];
    updated_closed.remove_if([ref](std::reference_wrapper<const V> &v){return v.get()==ref.get();});
    std::list<std::reference_wrapper<const V>> empty_list;
    options1.push_back(std::make_unique<Instance<T,V>>(total_time,current_time+1,valve[0].get(),true,tunnelmap.get(),updated_closed,empty_list,Instance<T,V>::Type::open));
  }
  if(type[0]!=Instance<T,V>::Type::end && closed_valves.size()>0) {
    auto links=tunnelmap.get().getConnection(valve[0].get());
    auto update_untouched=untouched_passed_valves[0];
    update_untouched.push_back(std::cref(valve[0].get()));
    for(auto it=links.begin();it!=links.end();it++) {
      if(std::find(untouched_passed_valves[0].begin(),untouched_passed_valves[0].end(),*it)==untouched_passed_valves[0].end()) {
        bool can_open=(std::find(closed_valves.begin(),closed_valves.end(),*it)!=closed_valves.end());
        options1.push_back(std::make_unique<Instance<T,V>>(total_time,current_time+1,it->get(),can_open,tunnelmap.get(),closed_valves,update_untouched,Instance<T,V>::Type::pass));
      }
    }
  }
  std::list<std::unique_ptr<Instance<T,V>>> options2;
  if(type[1]!=Instance<T,V>::Type::open && possible_to_open[1] && valve[1].get().getRate()>0 && (std::find(closed_valves.begin(),closed_valves.end(),valve[1].get())!=closed_valves.end())) {
    auto updated_closed=closed_valves;
    auto ref=valve[1];
    updated_closed.remove_if([ref](std::reference_wrapper<const V> &v){return v.get()==ref.get();});
    std::list<std::reference_wrapper<const V>> empty_list;
    options2.push_back(std::make_unique<Instance<T,V>>(total_time,current_time+1,valve[1].get(),true,tunnelmap.get(),updated_closed,empty_list,Instance<T,V>::Type::open));
  }
  if(type[1]!=Instance<T,V>::Type::end && closed_valves.size()>0) {
    auto links=tunnelmap.get().getConnection(valve[1].get());
    auto update_untouched=untouched_passed_valves[1];
    update_untouched.push_back(std::cref(valve[1].get()));
    for(auto it=links.begin();it!=links.end();it++) {
      if(std::find(untouched_passed_valves[1].begin(),untouched_passed_valves[1].end(),*it)==untouched_passed_valves[1].end()) {
        bool can_open=(std::find(closed_valves.begin(),closed_valves.end(),*it)!=closed_valves.end());
        options2.push_back(std::make_unique<Instance<T,V>>(total_time,current_time+1,it->get(),can_open,tunnelmap.get(),closed_valves,update_untouched,Instance<T,V>::Type::pass));
      }
    }
  }
  T current_max=0;
  if(options1.size()+options2.size()>0) {
    // sort list by maximum value, descending
    options1.sort([](const std::unique_ptr<Instance<T,V>>& a, const std::unique_ptr<Instance<T,V>>& b) {return a->lowerbound()>=b->lowerbound();});
    options2.sort([](const std::unique_ptr<Instance<T,V>>& a, const std::unique_ptr<Instance<T,V>>& b) {return a->lowerbound()>=b->lowerbound();});

    std::unique_ptr<InstanceWithElephant<T,V>> best;
    if(options1.size()==0) {
        for(auto it2=options2.begin();it2!=options2.end();it2++) {
            auto o1=std::make_unique<Instance<T,V>>(total_time,current_time+1,valve[0].get(),false,tunnelmap.get(),closed_valves,untouched_passed_valves[0],Instance<T,V>::Type::end);
            auto option=std::make_unique<InstanceWithElephant<T,V>>(*o1,**it2);
            if(it2==options2.begin())
                current_max=option->lowerbound();
            T ub=option->upperbound();
            if(ub<0)
                throw std::runtime_error("Bad implementation of upper bound");
            if(ub<to_exceed-own_contribution || current_max>ub) continue;
            T out=option->branch(current_max);
            if(out>ub)
                throw std::runtime_error("Bad implementation of upper bound");
            auto tmp=option->lowerbound();
            if(out<option->lowerbound())
                throw std::runtime_error("Bad implementation of lower bound");
            if(out>=current_max) {
                current_max=out;
                best=std::move(option);
                if(best==nullptr)
                    throw std::runtime_error("Undefined pointer");
            }
        }
    } else if(options2.size()==0) {
        for(auto it1=options1.begin();it1!=options1.end();it1++) {
            auto o2=std::make_unique<Instance<T,V>>(total_time,current_time+1,valve[1].get(),false,tunnelmap.get(),closed_valves,untouched_passed_valves[1],Instance<T,V>::Type::end);
            auto option=std::make_unique<InstanceWithElephant<T,V>>(**it1,*o2);
            if(it1==options1.begin())
                current_max=option->lowerbound();
            T ub=option->upperbound();
            if(ub<0)
                throw std::runtime_error("Bad implementation of upper bound");
            if(ub<to_exceed-own_contribution || current_max>ub) continue;
            T out=option->branch(current_max);
            if(out>ub)
                throw std::runtime_error("Bad implementation of upper bound");
            auto tmp=option->lowerbound();
            if(out<option->lowerbound())
                throw std::runtime_error("Bad implementation of lower bound");
            if(out>=current_max) {
                current_max=out;
                best=std::move(option);
                if(best==nullptr)
                    throw std::runtime_error("Undefined pointer");
            }
        }

    } else {
      for(auto it1=options1.begin();it1!=options1.end();it1++)
        for(auto it2=options2.begin();it2!=options2.end();it2++) {
            auto option=std::make_unique<InstanceWithElephant<T,V>>(**it1,**it2);
            if(it1==options1.begin()&&it2==options2.begin())
                current_max=option->lowerbound();
            T ub=option->upperbound();
            if(ub<0)
                throw std::runtime_error("Bad implementation of upper bound");
            if(ub<to_exceed-own_contribution || current_max>ub) continue;
            T out=option->branch(current_max);
            if(out<to_exceed-own_contribution || out<current_max) continue;
            if(out>ub)
                throw std::runtime_error("Bad implementation of upper bound");
            auto tmp=option->lowerbound();
            if(out<option->lowerbound())
                throw std::runtime_error("Bad implementation of lower bound");
            if(out>=current_max) {
                current_max=out;
                best=std::move(option);
                if(best==nullptr)
                    throw std::runtime_error("Undefined pointer");
            }
        }
    }

    if(best!=nullptr)
        next_instance=std::move(best);
    else
        next_instance=std::make_unique<InstanceWithElephant<T,V>>(total_time,current_time+1,valve[0].get(),false,valve[1].get(),false,tunnelmap.get(),closed_valves,untouched_passed_valves[0],untouched_passed_valves[1],Instance<T,V>::Type::end,Instance<T,V>::Type::end);
  } else {
      next_instance=std::make_unique<InstanceWithElephant<T,V>>(total_time,current_time+1,valve[0].get(),false,valve[1].get(),false,tunnelmap.get(),closed_valves,untouched_passed_valves[0],untouched_passed_valves[1],Instance<T,V>::Type::end,Instance<T,V>::Type::end);
  }
  return current_max+own_contribution;
}

template class Valve<TwoSymbol,int>;
template class Tunnelmap<Valve<TwoSymbol,int>>;
template class Instance<int,Valve<TwoSymbol,int>>;
template class InstanceWithElephant<int,Valve<TwoSymbol,int>>;
