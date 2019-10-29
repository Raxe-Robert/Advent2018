#include "advent2018.h"
#include "FileHelper.h"
#include "DateTime.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <regex>
#include <map>
#include <chrono>

/*
https://adventofcode.com/2018/day/4
*/
struct GuardAction {
	int guardId;
	DateTime dateTime;
	std::string action;

	GuardAction(int guardId, DateTime dateTime, std::string action) {
		this->guardId = guardId;
		this->dateTime = dateTime;
		this->action = action;
	}

	GuardAction() : guardId(0), dateTime(DateTime()), action("") {};
};

struct Guard {
	int guardId;
	std::vector<int> asleep;

	Guard(int guardId) {
		this->guardId = guardId;
		this->asleep.resize(60, 0);
	}

	Guard() : guardId(0) {};

	// Result is incremental on current values
	void SetAsleepTime(int from, int to) {
		for (int i = from; i < to; i++)
			asleep[i]++;
	}
};

void day04(input_t input)
{
	std::cout << "Day4 part 1" << std::endl;

	std::vector<std::string> data = FileHelper::ReadFileToStringVector("./input/day04.txt");

	std::regex dateTime_regex("\\[(\\d+)\\-(\\d+)\\-(\\d+) (\\d+)\\:(\\d+)\\] (.*)");
	std::regex guard_regex("(\\w+) \\#(\\d+)");

	std::vector<GuardAction> guardActions;

	std::smatch base_match;
	for (auto& line : data) {
		if (!regex_search(line, base_match, dateTime_regex))
			continue;

		int year = stoi(base_match[1]);
		int month = stoi(base_match[2]);
		int day = stoi(base_match[3]);
		int hour = stoi(base_match[4]);
		int minute = stoi(base_match[5]);
		std::string action = base_match[6];

		std::smatch guard_match;
		if (regex_search(action, guard_match, guard_regex))
			guardActions.push_back(GuardAction(stoi(guard_match[2]), DateTime(year, month, day, hour, minute), "begins shift"));
		else
			guardActions.push_back(GuardAction(-1, DateTime(year, month, day, hour, minute), action));
	}

	sort(guardActions.begin(), guardActions.end(), [](GuardAction i, GuardAction j) {
		return i.dateTime <= j.dateTime;
	});

	std::map<int, Guard> guards;
	int currentGuard;
	int startedSleeping;
	for (auto& x : guardActions) {
		if (x.guardId != -1) {
			if (guards.find(x.guardId) == guards.end())
				guards.insert(std::make_pair(x.guardId, Guard(x.guardId)));

			currentGuard = x.guardId;
		}
		else {
			if (x.action == "falls asleep")
				startedSleeping = x.dateTime.Minute();
			else if (x.action == "wakes up") {
				auto it = guards.find(currentGuard);
				if (it != guards.end())
					it->second.SetAsleepTime(startedSleeping, x.dateTime.Minute());
				else
					std::cout << "Failed - sorting probably went wrong" << std::endl;
			}
		}
	}

	Guard longestSleeper;
	int longestSleepTime = 0;
	int longestMinute;
	for (auto& guard : guards) {
		int sum = 0;
		int currLongestMinute = 0;

		for (int i = 0; i < guard.second.asleep.size(); i++) {
			sum += guard.second.asleep[i];
			if (guard.second.asleep[i] > guard.second.asleep[currLongestMinute])
				currLongestMinute = i;
		}
		if (sum > longestSleepTime) {
			longestSleepTime = sum;
			longestSleeper = guard.second;
			longestMinute = currLongestMinute;
		}
	}

	std::cout << "(Part1) Answer: " << longestSleeper.guardId * longestMinute << ", Guard ID: " << longestSleeper.guardId << ", Longest minute: " << longestMinute << std::endl;

	Guard sleptLongestOnMinute;
	int biggestDifference = 0;
	int indexOfBiggestDifference = 0;
	for (int i = 0; i < 60; i++) {		
		int indexOfSecondHighest = -1;
		int idOfHighest = -1;
		
		for (auto& guard : guards) {
			if (idOfHighest == -1 || guard.second.asleep[i] >= guards.at(idOfHighest).asleep[i]) {
				indexOfSecondHighest = idOfHighest;
				idOfHighest = guard.second.guardId;
			}
		}

		int difference = guards.at(idOfHighest).asleep[i] - guards.at(indexOfSecondHighest).asleep[i];
		if (difference >= biggestDifference) {
			sleptLongestOnMinute = idOfHighest;
			biggestDifference = difference;
			indexOfBiggestDifference = i;
		}
	}

	std::cout << "(Part2) Answer: " << sleptLongestOnMinute.guardId * indexOfBiggestDifference << ", Guard ID: " << sleptLongestOnMinute.guardId << ", Longest minute: " << indexOfBiggestDifference << std::endl;
}