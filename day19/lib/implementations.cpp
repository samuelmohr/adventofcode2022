#include "Storage.h"
#include "Blueprint.h"
#include "instance.h"

#include <regex>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <list>
#include <memory>

Instance::FactoryOptions::FactoryOptions()
	: Ore{true}, Clay{true}, Obsidian{true}, Geode{true}
{
}

Instance::FactoryOptions::FactoryOptions(bool o, bool c, bool ob, bool g)
	: Ore{o}, Clay{c}, Obsidian{ob}, Geode{g}
{
}

Instance::FactoryOptions::FactoryOptions(const Blueprint& bp, const Storage& s)
	: Ore{bp.canCreateOreRobot(s)},
	  Clay{bp.canCreateClayRobot(s)},
	  Obsidian{bp.canCreateObsidianRobot(s)},
	  Geode{bp.canCreateGeodeRobot(s)}
{
}

Instance::FactoryOptions Instance::FactoryOptions::invert(const FactoryOptions& fo)
{
	return FactoryOptions{!fo.Ore, !fo.Clay, !fo.Obsidian, !fo.Geode};
}

void Instance::FactoryOptions::mask(const FactoryOptions& m)
{
	Ore &= m.Ore;
	Clay &= m.Clay;
	Obsidian &= m.Obsidian;
	Geode &= m.Geode;
}

bool Instance::FactoryOptions::some() const
{
	return Ore || Clay || Obsidian || Geode;
}

Instance::Instance(const FactoryOptions config, const uint tt, const uint ct, const Storage& current_storage,
                   const RobotCollection& rc, const Blueprint& bp)
	: total_time{tt}, current_time{ct}, blueprint{std::cref(bp)}, start_storage{current_storage},
	  start_collection{rc}, factory{None}, factoryOptions{config}
{
}

Instance::Instance(const uint tt, const RobotCollection& rc, const Blueprint& bp)
	: total_time{tt}, current_time{1}, blueprint{std::cref(bp)},
	  start_storage{Storage{.ore = 0, .clay = 0, .obsidian = 0, .geode = 0}},
	  start_collection{rc}, factory{None}, factoryOptions{FactoryOptions{}}
{
}


uint Instance::size(const uint depth) const
{
	if (depth > 0)
	{
		// should have already branched
		if (next_instance == nullptr && options.size() == 0)
			return 0;

		if (next_instance != nullptr)
		{
			return next_instance->size(depth - 1);
		}

		uint sum = 0;
		auto it = options.begin();
		while (it != options.end())
		{
			sum += it->second->size(depth - 1);
			++it;
		}
		return sum;
	}
	return 1;
}

bool Instance::branch(const uint depth, const uint to_exceed)
{
	const uint own_production = start_collection.Geode;
	uint tmp = current_time;
	const uint subinstancesmust = to_exceed > own_production ? to_exceed - own_production : 0;

	if (depth > 0)
	{
		// should have already branched
		if (next_instance == nullptr && options.size() == 0)
			throw std::runtime_error("Branch step by step.");

		if (next_instance != nullptr)
		{
			bool success = next_instance->branch(depth - 1, subinstancesmust);
			if (!success)
				next_instance.reset();
			return success;
		}

		uint threshold = std::max(subinstancesmust, options.front().second->lowerbound());
		auto it = options.begin();
		while (it != options.end())
		{
			if (!it->second->branch(depth - 1, threshold))
				it = options.erase(it);
			else
				++it;
		}
		if (options.size() == 0)
			return false;

		options.sort(
			[](const std::pair<Robot, std::unique_ptr<Instance>>& a,
			   const std::pair<Robot, std::unique_ptr<Instance>>& b)
			{
				return a.second->lowerbound() > b.second->lowerbound();
			});
		uint current_max = options.front().second->lowerbound();

		it = options.begin();
		while (it != options.end())
		{
			uint ub = it->second->upperbound();
			if (ub < 0)
				throw std::runtime_error("Bad implementation of upper bound");
			if (ub >= current_max && ub >= subinstancesmust)
			{
				++it;
				continue;
			}
			// discard
			it = options.erase(it);
		}

		if (options.size() == 1)
		{
			auto it = options.begin();
			next_instance = std::move(it->second);
			factory = it->first;
			options.erase(it);
			return true;
		}
		return options.size() != 0;
	}

	// branch
	if (current_time == total_time)
		return true;

	// get options to continue
	FactoryOptions fo{blueprint.get(), start_storage};

	bool canBuild = fo.Ore && fo.Clay;
	// canBuild all tells whether we have resources to build everything that is possible. In this case, it does not make sense to 
	if (canBuild && start_collection.Clay > 0)
		canBuild = fo.Obsidian;
	if (canBuild && start_collection.Obsidian > 0)
		canBuild = fo.Geode;
	if (!canBuild)
	{
		// don't start factory
		Storage newstorage = start_storage;
		start_collection.produce(newstorage);
		options.push_back(std::make_pair(None, std::make_unique<Instance>(
			                                 FactoryOptions::invert(fo), total_time, current_time + 1, newstorage,
			                                 start_collection, blueprint.get())));
	}

	fo.mask(factoryOptions);
	if (fo.Ore)
	{
		Storage newstorage = start_storage;
		blueprint.get().createOreRobot(newstorage);
		start_collection.produce(newstorage);
		RobotCollection rc = start_collection;
		rc.Ore++;
		options.push_back(std::make_pair(
			Ore, std::make_unique<Instance>(FactoryOptions{}, total_time, current_time + 1, newstorage, rc,
			                                blueprint.get())));
	}
	if (fo.Clay)
	{
		Storage newstorage = start_storage;
		blueprint.get().createClayRobot(newstorage);
		start_collection.produce(newstorage);
		RobotCollection rc = start_collection;
		rc.Clay++;
		options.push_back(std::make_pair(
			Clay, std::make_unique<Instance>(FactoryOptions{}, total_time, current_time + 1, newstorage, rc,
			                                 blueprint.get())));
	}
	if (fo.Obsidian)
	{
		Storage newstorage = start_storage;
		blueprint.get().createObsidianRobot(newstorage);
		start_collection.produce(newstorage);
		RobotCollection rc = start_collection;
		rc.Obsidian++;
		options.push_back(std::make_pair(
			Obsidian, std::make_unique<Instance>(FactoryOptions{}, total_time, current_time + 1, newstorage, rc,
			                                     blueprint.get())));
	}
	if (fo.Geode)
	{
		Storage newstorage = start_storage;
		blueprint.get().createGeodeRobot(newstorage);
		start_collection.produce(newstorage);
		RobotCollection rc = start_collection;
		rc.Geode++;
		options.push_back(std::make_pair(
			Geode, std::make_unique<Instance>(FactoryOptions{}, total_time, current_time + 1, newstorage, rc,
			                                  blueprint.get())));
	}

	auto it = options.begin();
	while (it != options.end())
	{
		if (it->second->upperbound() < subinstancesmust)
			it = options.erase(it);
		else
			++it;
	}

	if (options.size() == 0)
		return false;

	if (options.size() == 1)
	{
		auto it = options.begin();
		next_instance = std::move(it->second);
		factory = it->first;
		options.erase(it);
	}
	else
	{
		// sort list by maximum value, descending
		options.sort([](const auto& a, const auto& b) { return a.second->lowerbound() > b.second->lowerbound(); });
	}
	return true;
}

uint Instance::lowerbound() const
{
	if (next_instance != nullptr)
		return next_instance->lowerbound() + start_collection.Geode;
	if (options.size() > 0)
		return options.front().second->lowerbound() + start_collection.Geode;

	// not branched yet
	uint val = (total_time + 1 - current_time) * start_collection.Geode;
	Storage s = start_storage;
	for (uint i = current_time; i < total_time; i++)
	{
		if (blueprint.get().createGeodeRobot(s))
			val += (total_time - i);
		start_collection.produce(s);
	}
	return val;
}

uint Instance::upperbound() const
{
	if (next_instance != nullptr)
		return next_instance->upperbound() + start_collection.Geode;
	if (options.size() > 0)
		return std::accumulate(options.begin(), options.end(), 0,
		                       [](uint val, const std::pair<Robot, std::unique_ptr<Instance>>& a)
		                       {
			                       uint tmp = a.second->upperbound();
			                       return (val < tmp) ? tmp : val;
		                       }) + start_collection.Geode;

	// not branched yet
	// assume you build a geode-producing robot every round (if you can).
	uint producedbycurrent = (total_time + 1 - current_time) * start_collection.Geode;
	uint diff = total_time + 1 - current_time;
	if (!blueprint.get().canCreateGeodeRobot(start_storage))
		diff--;
	return diff * (diff - 1) / 2 + producedbycurrent;
}

// if called after branch, then this Instance did some work and calculation, otherwise, undefined old data is printed.
std::string Instance::report() const
{
	std::string out = "== Minute " + std::to_string(current_time) + " ==\n";

	// status of factory
	options.sort(
		[](const std::pair<Robot, std::unique_ptr<Instance>>& a, const std::pair<Robot, std::unique_ptr<Instance>>& b)
		{
			return a.second->lowerbound() > b.second->lowerbound();
		});
	auto _fac = factory;
	if (options.size() > 0)
		_fac = options.front().first;
	else if (next_instance == nullptr)
		_fac = blueprint.get().canCreateGeodeRobot(start_storage) ? Geode : None;

	// factory
	switch (_fac)
	{
	case Ore:
		out += "  • The factory starts the production of a new ore-collecting robot.\n";
		break;
	case Clay:
		out += "  • The factory starts the production of a new clay-collecting robot.\n";
		break;
	case Obsidian:
		out += "  • The factory starts the production of a new obsidian-collecting robot.\n";
		break;
	case Geode:
		out += "  • The factory starts the production of a new geode-collecting robot.\n";
		break;
	case None:
		break;
	}

	// producer
	out += "  -> " + std::to_string(start_collection.Ore) + " ore robot" + (
		start_collection.Ore == 1 ? " is " : "s are ") + (start_collection.Ore == 0 ? "available;\n" : "producing;\n");
	out += "  -> " + std::to_string(start_collection.Clay) + " clay robot" + (
		start_collection.Clay == 1 ? " is " : "s are ") + (
		start_collection.Clay == 0 ? "available;\n" : "producing;\n");
	out += "  -> " + std::to_string(start_collection.Obsidian) + " obsidian robot" + (
		start_collection.Obsidian == 1 ? " is " : "s are ") + (start_collection.Obsidian == 0
			                                                       ? "available;\n"
			                                                       : "producing;\n");
	out += "  -> " + std::to_string(start_collection.Geode) + " geode robot" + (
		start_collection.Geode == 1 ? " is " : "s are ") + (start_collection.Geode == 0
			                                                    ? "available.\n"
			                                                    : "producing.\n");


	// storage
	Storage storage = start_storage;
	start_collection.produce(storage);
	switch (_fac)
	{
	case Ore:
		blueprint.get().createOreRobot(storage);
		break;
	case Clay:
		blueprint.get().createClayRobot(storage);
		break;
	case Obsidian:
		blueprint.get().createObsidianRobot(storage);
		break;
	case Geode:
		blueprint.get().createGeodeRobot(storage);
		break;
	}
	out += "  = The storage contains " + std::to_string(storage.ore) + " ore, " + std::to_string(storage.clay) +
		" clay, " + std::to_string(storage.obsidian) + " obsidian, and " + std::to_string(storage.geode) +
		" geodes. \n";

	// factory
	switch (_fac)
	{
	case Ore:
		out += "  • A new ore-collecting robot is ready.\n";
		break;
	case Clay:
		out += "  • A new clay-collecting robot is ready.\n";
		break;
	case Obsidian:
		out += "  • A new obsidian-collecting robot is ready.\n";
		break;
	case Geode:
		out += "  • A new geode-collecting robot is ready.\n";
		break;
	case None:
		out += "  • The factory is waiting for new orders. \n";
		break;
	}
	out += "\n";

	if (next_instance != nullptr)
		out += next_instance->report();
	else if (options.size() > 0)
		out += options.front().second->report();
	else if (current_time != total_time)
	{
		std::unique_ptr<Instance> instance;
		if (_fac == Geode)
		{
			RobotCollection rc = start_collection;
			rc.Geode++;
			instance = std::make_unique<Instance>(FactoryOptions{}, total_time, current_time + 1, storage, rc,
			                                      blueprint.get());
		}
		else
		{
			instance = std::make_unique<Instance>(FactoryOptions{}, total_time, current_time + 1, storage,
			                                      start_collection, blueprint.get());
		}
		out += instance->report();
	}
	return out;
}

void Instance::RobotCollection::produce(Storage& s) const
{
	s.ore += Ore;
	s.clay += Clay;
	s.obsidian += Obsidian;
	s.geode += Geode;
}


Blueprint::Blueprint(int n, unsigned int ore_robot, unsigned int clay_robot, unsigned int obsidian_robot[2],
                     unsigned int geode_robot[2])
	: number{n}, ore_robot_ore{ore_robot},
	  clay_robot_ore{clay_robot}, obsidian_robot_ore{obsidian_robot[0]},
	  obsidian_robot_clay{obsidian_robot[1]}, geode_robot_ore{geode_robot[0]},
	  geode_robot_obsidian{geode_robot[1]}
{
}

std::string Blueprint::getName() const
{
	return "Blueprint " + std::to_string(number);
}

int Blueprint::getNumber() const
{
	return number;
}

bool Blueprint::canCreateOreRobot(const Storage& s) const
{
	return s.ore >= ore_robot_ore;
}

bool Blueprint::canCreateClayRobot(const Storage& s) const
{
	return s.ore >= clay_robot_ore;
}

bool Blueprint::canCreateObsidianRobot(const Storage& s) const
{
	return s.ore >= obsidian_robot_ore && s.clay >= obsidian_robot_clay;
}

bool Blueprint::canCreateGeodeRobot(const Storage& s) const
{
	return s.ore >= geode_robot_ore && s.obsidian >= geode_robot_obsidian;
}

bool Blueprint::createOreRobot(Storage& s) const
{
	if (!canCreateOreRobot(s)) return false;
	s.ore -= ore_robot_ore;
	return true;
}

bool Blueprint::createClayRobot(Storage& s) const
{
	if (!canCreateClayRobot(s)) return false;
	s.ore -= clay_robot_ore;
	return true;
}

bool Blueprint::createObsidianRobot(Storage& s) const
{
	if (!canCreateObsidianRobot(s)) return false;
	s.ore -= obsidian_robot_ore;
	s.clay -= obsidian_robot_clay;
	return true;
}

bool Blueprint::createGeodeRobot(Storage& s) const
{
	if (!canCreateGeodeRobot(s)) return false;
	s.ore -= geode_robot_ore;
	s.obsidian -= geode_robot_obsidian;
	return true;
}

Blueprint* Blueprint::createFromString(std::string& s)
{
	std::regex blueprint_regex(
		"^Blueprint (\\d+): Each ore robot costs (\\d+) ore. Each clay robot costs (\\d+) ore. Each obsidian robot costs (\\d+) ore and (\\d+) clay. Each geode robot costs (\\d+) ore and (\\d+) obsidian.$",
		std::regex_constants::icase);
	std::smatch match;
	if (std::regex_search(s, match, blueprint_regex))
	{
		unsigned int data_or[2] = {
			static_cast<unsigned int>(stoi(match[4])), static_cast<unsigned int>(stoi(match[5]))
		};
		unsigned int data_gr[2] = {
			static_cast<unsigned int>(stoi(match[6])), static_cast<unsigned int>(stoi(match[7]))
		};
		return new Blueprint{
			stoi(match[1]), static_cast<unsigned int>(stoul(match[2])), static_cast<unsigned int>(stoul(match[3])),
			data_or, data_gr
		};
	}
	throw std::runtime_error("Cannot parse Blueprint");
}
