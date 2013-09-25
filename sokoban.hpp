#include <vector>
#include <string>
#include <algorithm>
#include "point.hpp"
#include "state.hpp"

struct Node {
	State state;
	char direction;
	Node *parent;
};

std::vector<Node*> possibleSteps(std::vector<std::string> map, Node *current);

void parseBoard(std::vector<std::string> &map, Node* root, std::vector<Point> &goal, std::vector<std::string> &clearBoard);

int hashState(State state);

bool isGoal(std::vector<Point> goal, State state);

State findPathTo(State start, Point goal);

std::string getPath(Node* node);