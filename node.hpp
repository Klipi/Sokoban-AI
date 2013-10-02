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

		bool hasBoxIn(const Point&);
		bool hasWallIn(const Point&);
		bool isFreePoint(const Point&);
		bool hasGoalIn(const Point&);
		Node* getChild(char, bool);
		//bool identifyDeadBox(Node*, Point&);
		bool isBoxStuck(const Point&, bool) const;
		bool isBoxStuck(const Point&) const;
		//bool identifyDeadBox(Point&);
		bool identifyDeadGroup(std::vector<Point>&, const Point&);
		std::vector<Point> getAdjacentBoxGroup(const Point&);

		Node():direction(' '),parent(NULL){};
		Node(State s, char d, Node* p):state(s), direction(d), parent(p){};

		std::vector<Node*> possibleSteps(const std::vector<std::string>& , bool);
		std::vector<Node*> getNextSteps(const std::vector<std::string>&);

	private:
		std::vector<Point> getMovableSides(const Point& box, const std::vector<std::string> &map);
		std::vector<Node*> findPaths(std::vector<Point> &goals, const std::vector<std::string> &map);
		bool isSearchTarget(std::vector<Point> &goals);
		void pushBoxes(std::vector<Node*>& nodes);

};

int heuristic(const State&);

struct NodeCompare
{
	bool operator()(const Node* a, const Node* b) const
	{
		return heuristic(a->state) > heuristic(b->state);
	}
};

#endif
