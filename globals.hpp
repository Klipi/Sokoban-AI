#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <string>
#include "point.hpp"

// Nobody likes global variables but we need it to compare states in the priority queue
extern std::vector<std::string> clearBoard;
extern std::vector<Point> goals;
extern std::vector<int**> distances;
extern int debug;

#endif
