#include "lib/instance.h"
#include "lib/Blueprint.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <iterator>
#include <numeric>

using namespace std;


list<std::unique_ptr<Blueprint>> blueprints;

void read(string filename)
{
	ifstream file(filename, ios::in);
	istream& stream = file;

	std::string line;
	while (getline(stream, line))
	{
		blueprints.push_back(std::unique_ptr<Blueprint>(Blueprint::createFromString(line)));
	}
	file.close();
}

void puzzle1_debug()
{
	cout << "PART 1:" << endl;
	const int total_time = 24;
	int score = 0;

	for (auto& bpp : blueprints)
	{
		auto problem = std::make_unique<Instance>(total_time, Instance::RobotCollection{
			                                          .Ore = 1, .Clay = 0, .Obsidian = 0, .Geode = 0
		                                          }, *bpp);
		uint branch_depth = 0;
		uint lowerbound;
		while (problem->upperbound() != (lowerbound = problem->lowerbound()))
		{
			cout << "Branch depth (" << problem->size(branch_depth) << " Instances): " << branch_depth <<
				"  with lowerbound " << lowerbound << " and upper bound " << problem->upperbound() << endl;
			problem->branch(branch_depth, lowerbound);
			branch_depth++;
		}
		cout << "Branch depth (" << problem->size(branch_depth) << " Instances): " << branch_depth <<
			"  with lowerbound " << lowerbound << " and upper bound " << problem->upperbound() << endl;
		cout << endl << "Collected geodes by " << bpp->getName() << ": " << lowerbound << endl;
		cout << problem->report() << endl << endl;

		score += bpp->getNumber() * lowerbound;
	}
	cout << endl << "The final score is " << score << endl;
}

void puzzle1()
{
	cout << "PART 1:" << endl;
	const int total_time = 24;

	int score = accumulate(begin(blueprints), end(blueprints), 0, [total_time](int total, const auto& bpp)
	{
		auto problem = std::make_unique<Instance>(total_time, Instance::RobotCollection{
			                                          .Ore = 1, .Clay = 0, .Obsidian = 0, .Geode = 0
		                                          }, *bpp);
		uint branch_depth = 0;
		uint lowerbound;
		while (problem->upperbound() != (lowerbound = problem->lowerbound()))
		{
			problem->branch(branch_depth, lowerbound);
			branch_depth++;
		}
		return total + bpp->getNumber() * lowerbound;
	});
	cout << endl << "The final score (accumulated quality levels) is " << score << endl;
}

void puzzle2()
{
	cout << endl << "PART 2:" << endl;
	const int total_time = 32;

	int score = accumulate(begin(blueprints), next(begin(blueprints), 3), 1, [total_time](int total, const auto& bpp)
	{
		auto problem = std::make_unique<Instance>(total_time, Instance::RobotCollection{
			                                          .Ore = 1, .Clay = 0, .Obsidian = 0, .Geode = 0
		                                          }, *bpp);
		uint branch_depth = 0;
		uint lowerbound;
		while (problem->upperbound() != (lowerbound = problem->lowerbound()))
		{
			problem->branch(branch_depth, lowerbound);
			branch_depth++;
		}
		return total * lowerbound;
	});
	cout << endl << "The final score (multiplied output) is " << score << endl;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "Provide input file as runtime argument";
		return 255;
	}

	read(argv[1]);
	puzzle1();
	puzzle2();
}
