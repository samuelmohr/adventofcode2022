#ifndef BLUEPRINT_H
#define BLUEPRINT_H

#include "Storage.h"
#include <string>

class Blueprint {
  private:
    const int number;
    const unsigned int ore_robot_ore,clay_robot_ore,obsidian_robot_ore,obsidian_robot_clay,geode_robot_ore,geode_robot_obsidian;

  public:
    Blueprint() = delete;
    Blueprint(int,unsigned int,unsigned int,unsigned int[2],unsigned int[2]);
    ~Blueprint() = default;

    std::string getName() const;
    int getNumber() const;

    bool canCreateOreRobot(const Storage &s) const;
    bool canCreateClayRobot(const Storage &s) const;
    bool canCreateObsidianRobot(const Storage &s) const;
    bool canCreateGeodeRobot(const Storage &s) const;

    bool createOreRobot(Storage &s) const;
    bool createClayRobot(Storage &s) const;
    bool createObsidianRobot(Storage &s) const;
    bool createGeodeRobot(Storage &s) const;

    static Blueprint* createFromString(std::string &s);
};

#endif // BLUEPRINT_H
