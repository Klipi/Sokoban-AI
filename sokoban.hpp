#ifndef SOKOBAN_H
#define SOKOBAN_H

#include <vector>
#include <string>
#include <algorithm>
#include "point.hpp"
#include "state.hpp"
#include "node.hpp"

void parseBoard(std::vector<std::string> &map, Node* root, std::vector<Point> &goal, std::vector<std::string> &clearBoard);

State findPathTo(State start, Point goal);

void showSolution(std::vector<std::string>, State, std::string);

std::string getPath(Node* node);

Node* findLowestPlayerPosition(Node*);

#endif
