#ifndef NODE_H
#define NODE_H

#include <vector>
#include "state.hpp"
#include "point.hpp"

class Node {
	public:
		State state;
		char direction;
		Node *parent;

		bool hasBoxIn(Point);
		bool hasWallIn(Point);
		bool hasGoalIn(Point);
		bool isFreePoint(Point);
		Node* getChild(char);
		//bool identifyDeadBox(Node*, Point);
		bool isBoxStuck(Point, bool);
		bool isBoxStuck(Point);
		//bool identifyDeadBox(Point);
		bool identifyDeadGroup(std::vector<Point>, Point);
		std::vector<Point> getAdjacentBoxGroup(Point);

		Node():direction(' '),parent(NULL){};
		Node(State s, char d, Node* p):state(s), direction(d), parent(p){};
};

int heuristic(State);

struct NodeCompare
{
	bool operator()(const Node* a, const Node* b) const
	{
		return heuristic(a->state) > heuristic(b->state);
	}
};

#endif
