#include "valve.h"
#include "twosymbol.h"
#include "tunnelmap.h"
#include "instance.h"
#include <numeric>
#include <iterator>
#include <algorithm>
#include <stdexcept>

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
    if(!possible_to_open || type==end || current_time==total_time)
        return 0;
    return (total_time-current_time-(type==open?0:1))*((T)valve.get());
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
T Instance<T,V>::branch() {
  if(current_time==total_time) {
      type=end;
      return 0;
  }

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
    std::unique_ptr<Instance<T,V>> best;
    for(auto it=options.begin();it!=options.end();it++) {
        T ub=(*it)->upperbound();
        if(ub<0)
            throw std::runtime_error("Bad implementation of upper bound");
      if(current_max>ub) continue;
      T out=(*it)->branch();
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
    next_instance=std::move(best);
  } else {
      next_instance=std::make_unique<Instance<T,V>>(total_time,current_time+1,valve.get(),false,tunnelmap.get(),closed_valves,untouched_passed_valves,Type::end);
  }
  return current_max+(type==open?(total_time-current_time)*((T)valve.get()):0);
}

template<class V>
bool operator==(const std::reference_wrapper<const V>& lhs, const std::reference_wrapper<const V>& rhs) {return lhs.get()==rhs.get();}



template class Valve<TwoSymbol,int>;
template class Tunnelmap<Valve<TwoSymbol,int>>;
template class Instance<int,Valve<TwoSymbol,int>>;
