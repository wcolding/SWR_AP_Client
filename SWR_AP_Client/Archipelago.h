#pragma once

#include <vector>
#include <string>

struct APServerInfo
{
	char server[64];
	char player[64];
	char pw[64];
};

struct CourseData
{
	std::string name;
	int slot;
	bool completed;
};

static std::vector<CourseData> completedCourses
{
	{"Amateur Race 1", 0, false},
	{"Amateur Race 2", 1, false},
	{"Amateur Race 3", 2, false},
	{"Amateur Race 4", 3, false},
	{"Amateur Race 5", 4, false},
	{"Amateur Race 6", 5, false},
	{"Amateur Race 7", 6, false},

	{"Semi-Pro Race 1", 8, false},
	{"Semi-Pro Race 2", 9, false},
	{"Semi-Pro Race 3", 10, false},
	{"Semi-Pro Race 4", 11, false},
	{"Semi-Pro Race 5", 12, false},
	{"Semi-Pro Race 6", 13, false},
	{"Semi-Pro Race 7", 14, false},

	{"Galactic Race 1", 16, false},
	{"Galactic Race 2", 17, false},
	{"Galactic Race 3", 18, false},
	{"Galactic Race 4", 19, false},
	{"Galactic Race 5", 20, false},
	{"Galactic Race 6", 21, false},
	{"Galactic Race 7", 22, false},

	{"Invitational Race 1", 24, false},
	{"Invitational Race 2", 25, false},
	{"Invitational Race 3", 26, false},
	{"Invitational Race 4", 27, false}
};