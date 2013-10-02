#ifndef SOKOBAN_H
#define SOKOBAN_H

#include <vector>
#include <string>
#include <algorithm>
#include "point.hpp"
#include "state.hpp"
#include "node.hpp"


void parseBoard(std::vector<std::string> &map, Node* root, std::vector<Point> &goal, std::vector<std::string> &clearBoard);

int hashState(const State& state);

bool isGoal(const std::vector<Point>& goal, State& state);

State findPathTo(const State& start, const Point& goal);

std::string getPath(Node* node);


#endif
