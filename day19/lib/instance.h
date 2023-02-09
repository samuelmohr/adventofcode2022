#ifndef INSTANCE_H
#define INSTANCE_H

typedef unsigned int uint;

#include "Storage.h"
#include "Blueprint.h"
#include <list>
#include <string>
#include <memory>

class Instance {
  public:
    enum Robot {Ore,Clay,Obsidian,Geode,None};
    struct RobotCollection {
      uint Ore,Clay,Obsidian,Geode;
      void produce(Storage &) const;
    };
    
  protected:
    class FactoryOptions {
      public:
        bool Ore,Clay,Obsidian,Geode;
        FactoryOptions();
        FactoryOptions(bool,bool,bool,bool);
        FactoryOptions(Blueprint const&, Storage const&);
        ~FactoryOptions() = default;
        static FactoryOptions invert(FactoryOptions const&);
        
        void mask(FactoryOptions const&);
        bool some() const;
    };

  private:
    const uint total_time;
    const uint current_time;
    const std::reference_wrapper<const Blueprint> blueprint;

    const Storage start_storage;
    const RobotCollection start_collection;
    Robot factory;
    FactoryOptions factoryOptions;

    std::unique_ptr<Instance> next_instance;
    mutable std::list<std::pair<Robot,std::unique_ptr<Instance>>> options;

  public:
    Instance() = delete;
    Instance(const uint total_time, const RobotCollection &collection, const Blueprint &blueprint);
    ~Instance() = default;
    Instance(const FactoryOptions config, const uint total_time, const uint current_time, const Storage &current_storage, const RobotCollection &collection, const Blueprint &blueprint);

  public:
    bool branch(const uint depth,const uint to_exceed);
    uint lowerbound() const;
    uint upperbound() const;
    std::string report() const;
    uint size(const uint depth) const;
};

#endif // INSTANCE_H
